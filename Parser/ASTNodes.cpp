#include "ASTNodes.h"

ASTIdentifierNode::ASTIdentifierNode(const std::string& name)
    : ASTNodeBase(ASTNodeBase::NodeType::IDENTIFIER)
{
    this->name = name;
};

ASTCreateDatabaseStmtNode::ASTCreateDatabaseStmtNode(ASTIdentifierNode *dbName)
    : ASTNodeBase(ASTNodeBase::NodeType::CREATE_DATABASE_STATEMENT)
{
    this->dbName = dbName;
}

ASTDropDatabaseStmtNode::ASTDropDatabaseStmtNode(ASTIdentifierNode *dbName)
    : ASTNodeBase(ASTNodeBase::NodeType::DROP_DATABASE_STATEMENT)
{
    this->dbName = dbName;
}

ASTSQLSmallIntDataType::ASTSQLSmallIntDataType()
    :ASTSQLDataType(ASTNodeBase::NodeType::TYPE_SMALL_INT)
{
}

ASTSQLIntegerDataType::ASTSQLIntegerDataType()
    : ASTSQLDataType(ASTNodeBase::NodeType::TYPE_INTEGER)
{
}

ASTSQLBigIntDataType::ASTSQLBigIntDataType()
    : ASTSQLDataType(ASTNodeBase::NodeType::TYPE_BIG_INT)
{
}

ASTCreateTableFieldNode::ASTCreateTableFieldNode(const std::string & name, ASTSQLDataType * dataType, ASTCreateTableFieldConstraint constraint)
    : ASTNodeBase(ASTNodeBase::NodeType::CREATE_TABLE_FIELD)
{
    this->name = name;
    this->dataType = dataType;
    this->constraint = constraint;
}

ASTCreateTableStmtNode::ASTCreateTableStmtNode(const std::list<ASTCreateTableFieldNode*>& fields)
    : ASTNodeBase(ASTNodeBase::NodeType::CREATE_TABLE_STATEMENT)
{
    this->fields = fields;
}
