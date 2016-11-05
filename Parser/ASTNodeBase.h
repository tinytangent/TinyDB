#ifndef __TINYDB_AST_NODE_BASE_H__
#define __TINYDB_AST_NODE_BASE_H__

class ASTNodeBase
{
public:
    enum class NodeType
    {
        IDENTIFIER,
        TYPE_SMALL_INT,
        TYPE_INTEGER,
        TYPE_BIG_INT,
        CREATE_TABLE_FIELD,
        CREATE_DATABASE_STATEMENT,
        DROP_DATABASE_STATEMENT,
        CREATE_TABLE_STATEMENT,
    };
protected:
    NodeType type;
    ASTNodeBase(NodeType type);
public:
    virtual NodeType getType();
};

#endif // __TINYDB_AST_NODE_BASE_H__
