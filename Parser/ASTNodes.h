#ifndef __TINYDB_AST_NODES_H__
#define __TINYDB_AST_NODES_H__

#include <string>
#include <list>
#include <vector>
#include "ASTNodeBase.h"

class ASTExpression;

class ASTFieldConstraintNode : public ASTNodeBase
{
public:
    enum Type
    {
        CONSTRAINT_NONE,
        CONSTRAINT_NOT_NULL,
        CONSTRAINT_NULL,
        CONSTRAINT_CHECK,
        CONSTRAINT_DEFAULT,
        CONSTRAINT_UNIQUE,
        CONSTRAINT_PRIMARY_KEY,
        CONSTRAINT_REFERENCES,
    };
    Type type;
    ASTExpression *expression;
    std::string tableName;
    std::string referenceTable;
    std::string referenceColumn;
    ASTFieldConstraintNode(Type type);
    ASTFieldConstraintNode(Type type, const std::string &tableName);
    ASTFieldConstraintNode(Type type, ASTExpression *expression,
        const std::string &referenceTable, const std::string &referenceColumn);
    ASTFieldConstraintNode(Type type, const std::string &tableName,
        ASTExpression *expression, const std::string &referenceTable,
        const std::string &referenceColumn);
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
        TYPE_STRING,
        TYPE_NULL,
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

class ASTSQLCharacterType : public ASTSQLDataType
{
public:
    const bool hasUnlimitedLength;
    const bool hasFixedLength;
    const std::string maxLength;
    ASTSQLCharacterType(bool hasUnlimitedLength,
        bool hasFixedLength, const std::string& maxLength);
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
        NOT_EQUAL,
        LESS_THAN,
        GREATER_THAN,
        LESS_THAN_OR_EQUAL,
        GREATER_THAN_OR_EQUAL,
        AND,
        OR,
        NOT,
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

class ASTShowDatabasesStmtNode : public ASTNodeBase
{
public:
    ASTShowDatabasesStmtNode();
};

class ASTUseDatabaseStmtNode : public ASTNodeBase
{
public:
    ASTIdentifierNode *dbName;
    ASTUseDatabaseStmtNode(ASTIdentifierNode *dbName);
};

class ASTCreateTableFieldNode : public ASTNodeBase
{
public:
    std::string name;
    ASTSQLDataType *dataType;
    std::list<ASTFieldConstraintNode*> constraints;
    ASTCreateTableFieldNode(
        const std::string &name, ASTSQLDataType *dataType,
        std::list<ASTFieldConstraintNode*> constraints
    );
};

class ASTCreateTableStmtNode : public ASTNodeBase
{
public:
    std::string name;
    std::list<ASTNodeBase*> fields;
    ASTCreateTableStmtNode(
        const std::string &name,
        const std::list<ASTNodeBase*> &fields);
};

class ASTInsertIntoStmtNode : public ASTNodeBase
{
public:
    std::string name;
    std::vector<std::list<ASTSQLDataValue*>> values;
    ASTInsertIntoStmtNode(
        const std::string &name,
        const std::vector<std::list<ASTSQLDataValue*>> values);
};

class ASTSelectColumnMap : public ASTNodeBase
{
public:
    const std::string columnName;
    ASTExpression *columnExpression;
    ASTSelectColumnMap( ASTExpression *columnExpression, const std::string &columnName);
};

class ASTSelectStmtNode : public ASTNodeBase
{
public:
    std::vector<ASTSelectColumnMap*> columnMap;
    std::string tableName;
    ASTExpression* expression;
    ASTSelectStmtNode(
        std::vector<ASTSelectColumnMap*> columnMap,
        const std::string &tableName,
        ASTExpression* expression);
};

class ASTDeleteStmtNode : public ASTNodeBase
{
public:
    const std::string tableName;
    ASTExpression const * expression;
    ASTDeleteStmtNode(const std::string &tableName, ASTExpression* expression);
};

class ASTUpdateStmtNode : public ASTNodeBase
{
public:
    const std::string tableName;
    const std::string columnName;
    const ASTExpression* updateExpression;
    const ASTExpression* whereExpression;
    ASTUpdateStmtNode(const std::string &tableName,
        const std::string &columnName, const ASTExpression *updateExpression,
        const ASTExpression *whereExpression);
};

class ASTDropTableStmtNode : public ASTNodeBase
{
public:
    std::string tableName;
    ASTDropTableStmtNode(const std::string& tableName);
};

class ASTShowTablesStmtNode : public ASTNodeBase
{
public:
    ASTShowTablesStmtNode();
};

class ASTCreateIndexStmtNode : public ASTNodeBase
{
public:
    const std::string indexName;
    const std::string tableName;
    const std::string columnName;
    ASTCreateIndexStmtNode(const std::string& indexName,
        const std::string& tableName, const std::string& columnName);
};

#endif // __TINYDB_AST_NODES_H___
