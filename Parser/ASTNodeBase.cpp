#include "ASTNodeBase.h"

ASTNodeBase::ASTNodeBase(ASTNodeBase::NodeType type)
{
    this->type = type;
}

ASTNodeBase::NodeType ASTNodeBase::getType()
{
    return type;
}
