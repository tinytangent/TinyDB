#include "BigIntFieldType.h"

const std::string BigIntFieldType::getTypeName() const
{
    return std::string("bigint");
}

FieldType* BigIntFieldType::construct(ASTNodeBase *astNode)
{
    return new BigIntFieldType();
}
