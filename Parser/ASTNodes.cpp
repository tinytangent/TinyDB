#include "boost/algorithm/string.hpp"
#include "ASTNodes.h"

ASTFieldConstraintNode::ASTFieldConstraintNode(Type type)
    :type(type), expression(nullptr)
{

}

ASTFieldConstraintNode::ASTFieldConstraintNode(
    Type type, ASTExpression *expression,
    const std::string &referenceTable, const std::string &referenceColumn)
    : type(type), expression(expression), referenceTable(referenceTable),
    referenceColumn(referenceColumn)
{
}

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

ASTCreateTableFieldNode::ASTCreateTableFieldNode(
    const std::string & name, ASTSQLDataType * dataType,
    std::list<ASTFieldConstraintNode*> constraints)
    : ASTNodeBase(ASTNodeBase::NodeType::CREATE_TABLE_FIELD),
    name(name), dataType(dataType), constraints(constraints)
{
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

ASTUseDatabaseStmtNode::ASTUseDatabaseStmtNode(ASTIdentifierNode * dbName)
    :ASTNodeBase(ASTNodeBase::NodeType::USE_DATABASE_STATEMENT)
{
    this->dbName = dbName;
}

ASTDropTableStmtNode::ASTDropTableStmtNode(const std::string & tableName)
    :ASTNodeBase(ASTNodeBase::NodeType::DROP_TABLE_STATEMENT),
    tableName(tableName)
{
}

ASTShowTablesStmtNode::ASTShowTablesStmtNode()
    :ASTNodeBase(ASTNodeBase::NodeType::SHOW_TABLES_STATEMENT)
{
}

ASTSQLCharacterType::ASTSQLCharacterType(bool hasUnlimitedLength, bool hasFixedLength, const std::string& maxLength)
    : ASTSQLDataType(ASTNodeBase::NodeType::TYPE_CHARACTER),
    hasUnlimitedLength(hasUnlimitedLength), hasFixedLength(hasFixedLength), maxLength(maxLength)
{
    name = "character";
}

ASTDeleteStmtNode::ASTDeleteStmtNode(const std::string & tableName, ASTExpression * expression)
    :ASTNodeBase(ASTNodeBase::NodeType::DELETE_STATEMENT),
    tableName(tableName), expression(expression)
{
}
