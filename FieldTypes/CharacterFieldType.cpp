#include <algorithm>
#include "Parser/ASTNodes.h"
#include "CharacterFieldType.h"

FieldType * CharacterFieldType::construct(ASTNodeBase * astNode, AbstractDynamicAllocator *dynamicAllocator)
{
    auto typeNode = (ASTSQLCharacterType*)astNode;
    CharacterFieldType* ret = new CharacterFieldType();
    ret->hasUnlimitedLength = typeNode->hasUnlimitedLength;
    ret->hasFixedLength = typeNode->hasFixedLength;
    ret->maxLength = std::stoi(typeNode->maxLength);
    return ret;
}

FieldType * CharacterFieldType::fromBinary(char * buffer, int length, AbstractDynamicAllocator *dynamicAllocator)
{
    CharacterFieldType* ret = new CharacterFieldType();
    ret->hasUnlimitedLength = (buffer[0] != 0x00);
    ret->hasFixedLength = (buffer[0] != 0x00);
    ret->maxLength = *(uint32_t*)(buffer + 2);
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
    return maxLength > 32 ? maxLength : 32;
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
        //TODO: handle variable storage
        memcpy(buffer, valueNode->value.c_str(),
            std::min((int)valueNode->value.length(), constantLength));
    }
    else
    {
        //TODO: handle variable storage
        uint32_t length = valueNode->value.length();
        auto constantLength = getConstantLength();
        memcpy(buffer, &length, sizeof(uint32_t));
        memcpy(buffer + 4, valueNode->value.c_str(), std::min(length, (uint32_t)constantLength - 4));
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
