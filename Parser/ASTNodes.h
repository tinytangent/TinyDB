#ifndef __TINYDB_AST_NODES_H__
#define __TINYDB_AST_NODES_H__

#include <string>
#include <list>
#include "ASTNodeBase.h"

enum ASTCreateTableFieldConstraint
{
    CONSTRAINT_NONE,
    CONSTRAINT_UNIQUE,
    CONSTRAINT_NULL,
    CONSTRAINT_NOT_NULL,
};

class ASTSQLDataType : public ASTNodeBase
{
public:
    std::string name;
    using ASTNodeBase::ASTNodeBase;
};

class ASTSQLDataValue : public ASTNodeBase
{
public:
    enum Type
    {
        TYPE_NUMERICAL,
        TYPE_STRING
    };
public:
    Type dataType;
    std::string value;
    ASTSQLDataValue(Type dataType, const std::string &value);
};

class ASTSQLSmallIntDataType : public ASTSQLDataType
{
public:
    ASTSQLSmallIntDataType();
};

class ASTSQLIntegerDataType : public ASTSQLDataType
{
public:
    ASTSQLIntegerDataType();
};

class ASTSQLBigIntDataType : public ASTSQLDataType
{
public:
    ASTSQLBigIntDataType();
};

class ASTIdentifierNode : public ASTNodeBase
{
public:
    std::string name;
    ASTIdentifierNode(const std::string& name);
};

class ASTExpression : public ASTNodeBase
{
public:
    enum Operator {
        NONE_CONSTANT, //For leaf node
        NONE_COLUMN_NAME,
        ADD,
        MINUS,
        MULTIPLY,
        DIVIDE,
        MOD,
        EQUAL,
        LESS_THAN,
        GREATER_THAN,
    };
public:
    union {
        ASTIdentifierNode *identifier;
        ASTSQLDataValue *constant;
    };
    ASTExpression* left;
    ASTExpression* right;
    Operator op;
    ASTExpression(Operator op, ASTExpression* left, ASTExpression* right);
    ASTExpression(ASTIdentifierNode* identifier);
    ASTExpression(ASTSQLDataValue* dataValue);
};

class ASTCreateDatabaseStmtNode : public ASTNodeBase
{
public:
    ASTIdentifierNode *dbName;
    ASTCreateDatabaseStmtNode(ASTIdentifierNode *dbName);
};

class ASTDropDatabaseStmtNode : public ASTNodeBase
{
public:
    ASTIdentifierNode *dbName;
    ASTDropDatabaseStmtNode(ASTIdentifierNode *dbName);
};

class ASTCreateTableFieldNode : public ASTNodeBase
{
public:
    std::string name;
    ASTSQLDataType *dataType;
    ASTCreateTableFieldConstraint constraint;
    ASTCreateTableFieldNode(
        const std::string &name, ASTSQLDataType *dataType,
        ASTCreateTableFieldConstraint constraint
    );
};

class ASTCreateTableStmtNode : public ASTNodeBase
{
public:
    std::string name;
    std::list<ASTCreateTableFieldNode*> fields;
    ASTCreateTableStmtNode(
        const std::string &name,
        const std::list<ASTCreateTableFieldNode*> &fields);
};

class ASTInsertIntoStmtNode : public ASTNodeBase
{
public:
    std::string name;
    std::list<ASTSQLDataValue*> values;
    ASTInsertIntoStmtNode(
        const std::string &name,
        const std::list<ASTSQLDataValue*> values);
};

class ASTSelectStmtNode : public ASTNodeBase
{
public:
    std::string tableName;
    ASTExpression* expression;
    ASTSelectStmtNode(
        const std::string &tableName,
        ASTExpression* expression);
};

#endif // __TINYDB_AST_NODES_H___
