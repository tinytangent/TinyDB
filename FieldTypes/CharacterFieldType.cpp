#include <algorithm>
#include "Parser/ASTNodes.h"
#include "CharacterFieldType.h"
#include "Storage\BuddyDynamicAllocator.h"
FieldType * CharacterFieldType::construct(ASTNodeBase * astNode, AbstractDynamicAllocator *dynamicAllocator)
{
    auto typeNode =
        (ASTSQLCharacterType*)(((ASTCreateTableFieldNode*)astNode)->dataType);
    CharacterFieldType* ret = new CharacterFieldType();
    ret->hasUnlimitedLength = typeNode->hasUnlimitedLength;
    ret->hasFixedLength = typeNode->hasFixedLength;
    ret->maxLength = std::stoi(typeNode->maxLength);
    ret->dynamicAllocator = dynamicAllocator;
    ret->storageArea = dynamicAllocator->getStorageArea();
    return ret;
}

FieldType * CharacterFieldType::fromBinary(char * buffer, int length, AbstractDynamicAllocator *dynamicAllocator)
{
    CharacterFieldType* ret = new CharacterFieldType();
    ret->hasUnlimitedLength = (buffer[0] != 0x00);
    ret->hasFixedLength = (buffer[0] != 0x00);
    ret->maxLength = *(uint32_t*)(buffer + 2);
    ret->dynamicAllocator = dynamicAllocator;
    ret->storageArea = dynamicAllocator->getStorageArea();
    return ret;
}

const std::string CharacterFieldType::getTypeName() const
{
    return std::string("character");
}

bool CharacterFieldType::hasConstantLength()
{
    if (hasUnlimitedLength) return false;
    if (hasFixedLength)
    {
        return maxLength <= 32;
    }
    return maxLength <= 28;
}

int CharacterFieldType::getConstantLength()
{
    if(hasUnlimitedLength) return 32;
    int ret;
    if (hasFixedLength)
    {
        ret = maxLength;
    }
    else
    {
        ret = maxLength + 4;
    }
    return ret < 32 ? ret : 32;
}

int CharacterFieldType::getHeaderLength()
{
    return 1 + 1 + 4; //TODO : remove those magic numbers.
}

void CharacterFieldType::writeHeader(char * buffer)
{
    buffer[0] = hasUnlimitedLength ? 0x01 : 0x00;
    buffer[1] = hasFixedLength ? 0x01 : 0x00;
    *(uint32_t*)(buffer + 2) = maxLength;
}

int CharacterFieldType::parseASTNode(ASTNodeBase* node, char* buffer)
{
    auto valueNode = (ASTSQLDataValue*)node;
    auto constantLength = getConstantLength();
    if (hasFixedLength)
    {
		if (hasConstantLength())
		{
			memcpy(buffer, valueNode->value.c_str(), std::min((int)valueNode->value.length(), constantLength));
		}
		else
		{
			uint32_t loc = dynamicAllocator->allocate(valueNode->value.size());
			uint32_t length = valueNode->value.length();
			memcpy(buffer, &loc, sizeof(uint32_t));
			char *text = new char[valueNode->value.length()];
			memcpy(text, valueNode->value.c_str(), sizeof(valueNode->value));
			storageArea->setDataAt(loc, text, valueNode->value.size());
			delete(text);
		}
    }
    else
    {
		if (hasConstantLength())
		{
			uint32_t length = valueNode->value.length();
			auto constantLength = getConstantLength();
			memcpy(buffer, &length, sizeof(uint32_t));
			memcpy(buffer + 4, valueNode->value.c_str(), std::min(length, (uint32_t)constantLength - 4));
		}
		else
		{
			uint32_t loc = dynamicAllocator->allocate(valueNode->value.size() + 1);
			uint32_t length = valueNode->value.length();
			std::cout << "length = " << length << " loc =  " << loc;
			memcpy(buffer, &length, sizeof(uint32_t));
			memcpy(buffer + 4, &loc, sizeof(uint32_t));
			char *text = new char[length + 1];
			memcpy(text, valueNode->value.c_str(), length);
            text[length] = '\0';
			storageArea->setDataAt(loc, text, valueNode->value.size());
			delete(text);
		}

    }
    return getConstantLength();
}

bool CharacterFieldType::isEqual(char *buffer1, char *buffer2)
{
    return memcmp(buffer1, buffer2, getConstantLength()) == 0;
}

bool CharacterFieldType::isGreaterThan(char *, char *)
{
    //TODO: Not finished.
    return false;
}

std::string CharacterFieldType::ToStringValue(char *binaryStream, int length)
{
    if (!hasFixedLength)
    {
        if (hasConstantLength())
        {
            uint32_t size = *(uint32_t*)binaryStream;
            binaryStream += sizeof(uint32_t);
            return std::string(binaryStream, size);
        }
        else
        {
            uint32_t size = *(uint32_t*)binaryStream;
            uint32_t loc = *(uint32_t*)(binaryStream + 4);
            char *text = new char[size + 1];
            storageArea->getDataAt(loc, text, size + 1);
            std::string ret = std::string(text);
            delete[] text;
            return ret;
        }
    }
    else
    {
        if (hasConstantLength())
        {
            return std::string(binaryStream, maxLength);
        }
        else
        {
            uint32_t loc = *(uint32_t*)(binaryStream);
            char *text = new char[maxLength + 1];
            storageArea->getDataAt(loc, text, maxLength);
            std::string ret = std::string(text);
            delete[] text;
            return ret;
        }
    }
}
SQLValue CharacterFieldType::dataValue(char* buffer)
{
    return ToStringValue(buffer, 0);
}
int CharacterFieldType::parseSQLValue(const SQLValue& sqlValue, char* buffer)
{
    //TODO......
    *buffer = *(sqlValue.stringValue.c_str());
    return sizeof(buffer);
}
