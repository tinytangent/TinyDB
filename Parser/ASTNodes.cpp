#include "ASTNodes.h"

ASTIdentifierNode::ASTIdentifierNode(const std::string& name)
    : ASTNodeBase(ASTNodeBase::NodeType::IDENTIFIER)
{
    this->name = name;
};

ASTCreateDatabaseStmtNode::ASTCreateDatabaseStmtNode(ASTIdentifierNode *dbName)
    : ASTNodeBase(ASTNodeBase::NodeType::CREATE_DATABASE)
{
    this->dbName = dbName;
}

ASTDropDatabaseStmtNode::ASTDropDatabaseStmtNode(ASTIdentifierNode *dbName)
    : ASTNodeBase(ASTNodeBase::NodeType::DROP_DATABASE)
{
    this->dbName = dbName;
}
