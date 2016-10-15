#ifndef __TINYDB_AST_NODES_H__
#define __TINYDB_AST_NODES_H__

#include <string>
#include "ASTNodeBase.h"

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

#endif // __TINYDB_AST_NODES_H___
