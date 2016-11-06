#ifndef __TINYDB_FIELD_TYPE_H__
#define __TINYDB_FIELD_TYPE_H__

#include <map>
#include <string>

class ASTNodeBase;

/**
 * The FieldType class defines an abstract data type in the database,
 * For example, VARCHAR, INTEGER, etc.
 */
class FieldType
{
protected:
    static std::map<std::string, FieldType*> registeredTypes;
public:
    static bool registerType(const std::string &typeName, FieldType *type);
    static FieldType *getType(const std::string &typeName);
public:
    virtual FieldType* construct(ASTNodeBase *astNode) = 0;
    virtual const std::string getTypeName() const = 0;
    virtual bool hasConstantLength() = 0;
    virtual int getConstantLength() = 0;
    virtual int compare(char* buffer, ASTNodeBase* node) = 0;

    /**
     * Get the header length of this data type. The "header" doesn't contains
     * things like field name and type name, they are handled by FieldList.
     * Instead, it only contains data type specific information,
     * i.e. n in VARCHAR(n).
     */
    virtual int getHeaderLength() = 0;

    virtual void writeHeader(char *buffer) = 0;
    virtual int getDataLength(ASTNodeBase* node) = 0;
    virtual int toBinary(ASTNodeBase *node, char* buffer) = 0;
    virtual std::string ToStringValue(char *binaryStream, int length) = 0;
    virtual int parseHeader(char* buffer) = 0;
};

#endif // __TINYDB_FIELD_TYPE_H__
