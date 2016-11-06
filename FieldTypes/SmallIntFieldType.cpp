#include "SmallIntFieldType.h"

const std::string SmallIntFieldType::getTypeName() const
{
    return std::string("smallint");
}

FieldType* SmallIntFieldType::construct(ASTNodeBase *astNode)
{
    return new SmallIntFieldType();
}

FieldType* SmallIntFieldType::fromBinary(char *buffer, int length)
{
    return new SmallIntFieldType();
}
