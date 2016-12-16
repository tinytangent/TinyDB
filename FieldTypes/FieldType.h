#ifndef __TINYDB_FIELD_TYPE_H__
#define __TINYDB_FIELD_TYPE_H__

#include <map>
#include <string>

class ASTNodeBase;
class AbstractDynamicAllocator;
class AbstractStorageArea;

/**
 * The FieldType class defines an abstract data type in the database,
 * For example, VARCHAR, INTEGER, etc.
 */
class FieldType
{
protected:
    static std::map<std::string, FieldType*> registeredTypes;
    AbstractDynamicAllocator *dynamicAllocator;
    AbstractStorageArea *storageArea;
public:
    static bool registerType(const std::string &typeName, FieldType *type);
    static FieldType *getType(const std::string &typeName);
public:
    virtual FieldType* construct(ASTNodeBase *astNode, AbstractDynamicAllocator *dynamicAllocator) = 0;
    virtual FieldType* fromBinary(char *buffer, int length, AbstractDynamicAllocator *dynamicAllocator) = 0;
    virtual const std::string getTypeName() const = 0;
    virtual bool hasConstantLength() = 0;
    virtual int getConstantLength() = 0;

    // Those functions are used to perform field comparison
    virtual bool isGreaterThan(char* buffer1, char* buffer2) = 0;
    virtual bool isEqual(char* buffer1, char* buffer2) = 0;
    virtual bool isLessThan(char* buffer1, char* buffer2);
    virtual bool isNotEqual(char* buffer1, char* buffer2);
    virtual bool isGreaterThanOrEqual(char* buffer1, char* buffer2);
    virtual bool isLessThanOrEqual(char* buffer1, char* buffer2);

    /**
     * Get the header length of this data type. The "header" doesn't contains
     * things like field name and type name, they are handled by FieldList.
     * Instead, it only contains data type specific information,
     * i.e. n in VARCHAR(n).
     */
    virtual int getHeaderLength() = 0;

    virtual int parseASTNode(ASTNodeBase* node, char* buffer) = 0;
    virtual void writeHeader(char *buffer) = 0;
    virtual std::string ToStringValue(char *binaryStream, int length) = 0;
};

#endif // __TINYDB_FIELD_TYPE_H__
