#include "SmallIntFieldType.h"

const std::string SmallIntFieldType::getTypeName() const
{
    return std::string("smallint");
}

FieldType* SmallIntFieldType::construct(ASTNodeBase *astNode, AbstractDynamicAllocator *dynamicAllocator)
{
    return new SmallIntFieldType();
}

FieldType* SmallIntFieldType::fromBinary(char *buffer, int length, AbstractDynamicAllocator *dynamicAllocator)
{
    return new SmallIntFieldType();
}
