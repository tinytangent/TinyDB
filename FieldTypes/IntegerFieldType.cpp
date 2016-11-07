#include "IntegerFieldType.h"

const std::string IntegerFieldType::getTypeName() const
{
    return std::string("integer");
}

FieldType* IntegerFieldType::construct(ASTNodeBase *astNode)
{
    return new IntegerFieldType();
}

FieldType* IntegerFieldType::fromBinary(char *buffer, int length)
{
    return new IntegerFieldType();
}