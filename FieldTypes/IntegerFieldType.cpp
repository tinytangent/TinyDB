#include "IntegerFieldType.h"

const std::string IntegerFieldType::getTypeName() const
{
    return std::string("integer");
}

FieldType* IntegerFieldType::construct(ASTNodeBase *astNode, AbstractDynamicAllocator *dynamicAllocator)
{
    return new IntegerFieldType();
}

FieldType* IntegerFieldType::fromBinary(char *buffer, int length, AbstractDynamicAllocator *dynamicAllocator)
{
    return new IntegerFieldType();
}
