#ifndef __TINYDB_CHARACTER_FIELD_TYPE_H__
#define __TINYDB_CHARACTER_FIELD_TYPE_H__

#include "FieldType.h"

class CharacterFieldType : public FieldType
{
protected:
    bool hasUnlimitedLength;
    bool hasFixedLength;
    uint32_t maxLength;
    BuddyDynamicAllocator *allocator;
	AbstractStorageArea *storageArea;
public:
    FieldType* construct(ASTNodeBase *astNode, AbstractDynamicAllocator *dynamicAllocator) override;
    FieldType* fromBinary(char *buffer, int length, AbstractDynamicAllocator *dynamicAllocator) override;
    const std::string getTypeName() const override;
    bool hasConstantLength() override;
    int getConstantLength() override;
    int getHeaderLength() override;
    void writeHeader(char *buffer) override;
    int parseASTNode(ASTNodeBase *, char *) override;
    bool isEqual(char *, char *) override;
    bool isGreaterThan(char *, char *) override;
    virtual std::string ToStringValue(char *binaryStream, int length)
    {
        if(!hasFixedLength)
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
				char *text = new char[size];
				storageArea->getDataAt(loc, text, size);
				return std::string(text);
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
				char *text = new char[maxLength];
				storageArea->getDataAt(loc, text, maxLength);
				return std::string(text);
			}
        }
    }
    SQLValue dataValue(char* buffer) override
    {
        return ToStringValue(buffer, 0);
    }
    int parseSQLValue(const SQLValue& sqlValue, char* buffer) override
    {
        //TODO......
		*buffer = *(sqlValue.stringValue.c_str());
        return sizeof(buffer);
    }
};

#endif
