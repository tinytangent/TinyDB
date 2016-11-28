#include "boost/algorithm/string.hpp"
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

ASTSQLDataValue::ASTSQLDataValue(Type type, const std::string &value)
    :ASTNodeBase(ASTNodeBase::NodeType::DATA_VALUE)
{
    this->dataType = type;
    this->value = value;
}

ASTSQLSmallIntDataType::ASTSQLSmallIntDataType()
    :ASTSQLDataType(ASTNodeBase::NodeType::TYPE_SMALL_INT)
{
    name = "smallint";
}

ASTSQLIntegerDataType::ASTSQLIntegerDataType()
    : ASTSQLDataType(ASTNodeBase::NodeType::TYPE_INTEGER)
{
    name = "integer";
}

ASTSQLBigIntDataType::ASTSQLBigIntDataType()
    : ASTSQLDataType(ASTNodeBase::NodeType::TYPE_BIG_INT)
{
    name = "bigint";
}

ASTCreateTableFieldNode::ASTCreateTableFieldNode(const std::string & name, ASTSQLDataType * dataType, ASTCreateTableFieldConstraint constraint)
    : ASTNodeBase(ASTNodeBase::NodeType::CREATE_TABLE_FIELD)
{
    this->name = name;
    this->dataType = dataType;
    this->constraint = constraint;
}

ASTExpression::ASTExpression(Operator op, ASTExpression* left, ASTExpression* right)
    : ASTNodeBase(ASTNodeBase::NodeType::EXPRESSION)
{
    this->op = op;
    this->left = left;
    this->right = right;
}

ASTExpression::ASTExpression(ASTIdentifierNode* identifier)
    : ASTNodeBase(ASTNodeBase::NodeType::EXPRESSION)
{
    this->op = ASTExpression::NONE_COLUMN_NAME;
    this->identifier = identifier;
    this->left = this->right = nullptr;
}

ASTExpression::ASTExpression(ASTSQLDataValue* dataValue)
    : ASTNodeBase(ASTNodeBase::NodeType::EXPRESSION)
{
    this->op = ASTExpression::NONE_CONSTANT;
    this->constant = dataValue;
    this->left = this->right = nullptr;
}

ASTCreateTableStmtNode::ASTCreateTableStmtNode(
    const std::string& name,
    const std::list<ASTCreateTableFieldNode*>& fields
) : ASTNodeBase(ASTNodeBase::NodeType::CREATE_TABLE_STATEMENT)
{
    this->name = name;
    this->fields = fields;
}

ASTInsertIntoStmtNode::ASTInsertIntoStmtNode(const std::string & name, const std::list<ASTSQLDataValue*> values)
    :ASTNodeBase(ASTNodeBase::NodeType::INSERT_INTO_STATEMENT)
{
    this->name = name;
    this->values = values;
}

ASTSelectStmtNode::ASTSelectStmtNode(
    const std::string &tableName, ASTExpression* expression
) : ASTNodeBase(ASTNodeBase::NodeType::SELECT_STATEMENT)
{
    this->tableName = tableName;
    this->expression = expression;
}

ASTShowDatabasesStmtNode::ASTShowDatabasesStmtNode()
    :ASTNodeBase(ASTNodeBase::NodeType::SHOW_DATABASES_STATEMENT)
{
}
