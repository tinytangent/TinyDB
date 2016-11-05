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
    using ASTNodeBase::ASTNodeBase;
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
    std::list<ASTCreateTableFieldNode*> fields;
    ASTCreateTableStmtNode(const std::list<ASTCreateTableFieldNode*> &fields);
};

#endif // __TINYDB_AST_NODES_H___
