#ifndef __TINYDB_FIXED_LENGTH_NUMERICAL_TYPE_H__
#define __TINYDB_FIXED_LENGTH_NUMERICAL_TYPE_H__

#include "Parser/ASTNodes.h"
#include "FieldType.h"

template<class T>
class FixedLengthIntegerFieldType : public FieldType
{
public:
    virtual bool hasConstantLength() override;
    virtual int getConstantLength() override;
    virtual int parseIntegerValue(std::string& integerValue, char* buffer);
    virtual int parseStringValue(std::string& stringValue, char* buffer);
    virtual int parseASTNode(ASTNodeBase* node, char* buffer);
    virtual std::string ToStringValue(char *binaryStream, int length)
    {
        return std::to_string(*(T*)binaryStream);
    }
    virtual int getHeaderLength();
    virtual void writeHeader(char *buffer);

    virtual bool isGreaterThan(char* buffer1, char* buffer2)
    {
        return (*(T*)buffer1) > (*(T*)buffer2);
    }

    virtual bool isLessThan(char* buffer1, char* buffer2)
    {
        return (*(T*)buffer1) < (*(T*)buffer2);
    }

    virtual bool isEqual(char* buffer1, char* buffer2)
    {
        return (*(T*)buffer1) == (*(T*)buffer2);
    }

    virtual bool isNotEqual(char* buffer1, char* buffer2)
    {
        return (*(T*)buffer1) != (*(T*)buffer2);
    }

    virtual bool isGreaterThanOrEqual(char* buffer1, char* buffer2)
    {
        return (*(T*)buffer1) >= (*(T*)buffer2);
    }

    virtual bool isLessThanOrEqual(char* buffer1, char* buffer2)
    {
        return (*(T*)buffer1) <= (*(T*)buffer2);
    }

    virtual int getDataLength(ASTNodeBase* node)
    {
        //TODO
        return 0;
    }
    virtual int toBinary(ASTNodeBase *node, char* buffer)
    {
        //TODO
        return 0;
    }
    virtual int parseHeader(char* buffer)
    {
        //TODO
        return 0;
    }
};


template<class T>
bool FixedLengthIntegerFieldType<T>::hasConstantLength()
{
    return true;
}

template<class T>
int FixedLengthIntegerFieldType<T>::getConstantLength()
{
    return sizeof(T);
}

template<class T>
int FixedLengthIntegerFieldType<T>::parseIntegerValue(std::string& integerValue, char* buffer)
{
    //TODO: Handle integer type with different width!
    T data = (T)std::stoll(integerValue.c_str());
    *(T*)buffer = data;
    return sizeof(T);
}

template<class T>
int FixedLengthIntegerFieldType<T>::parseStringValue(std::string& stringValue, char* buffer)
{
    //TODO: Handle integer type with different width!
    //TODO: Does SQL allow this?
    T data = (T)std::stoll(stringValue.c_str());
    *(T*)buffer = data;
    return sizeof(T);
}

template<class T>
int FixedLengthIntegerFieldType<T>::parseASTNode(ASTNodeBase* node, char* buffer)
{
    auto valueNode = dynamic_cast<ASTSQLDataValue*>(node);
    if (valueNode == nullptr)
    {
        return -1;
    }
    T data = (T)std::stoll(valueNode->value.c_str());
    *(T*)buffer = data;
    return sizeof(T);
}

template<class T>
int FixedLengthIntegerFieldType<T>::getHeaderLength()
{
    return 0;
}

template<class T>
void FixedLengthIntegerFieldType<T>::writeHeader(char *buffer)
{
}

#endif // __TINYDB_FIXED_LENGTH_NUMERICAL_TYPE_H__
