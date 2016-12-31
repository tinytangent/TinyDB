#ifndef __TINYDB_CHARACTER_FIELD_TYPE_H__
#define __TINYDB_CHARACTER_FIELD_TYPE_H__

#include "FieldType.h"

class BuddyDynamicAllocator;

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
    virtual std::string ToStringValue(char *binaryStream, int length);
    SQLValue dataValue(char* buffer) override;
    int parseSQLValue(const SQLValue& sqlValue, char* buffer) override;
};

#endif
