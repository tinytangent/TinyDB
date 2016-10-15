#ifndef __TINYDB_AST_NODE_BASE_H__
#define __TINYDB_AST_NODE_BASE_H__

class ASTNodeBase
{
public:
    enum class NodeType
    {
        IDENTIFIER = 1,
        CREATE_DATABASE = 2,
        DROP_DATABASE = 3
    };
protected:
    NodeType type;
    ASTNodeBase(NodeType type);
public:
    virtual NodeType getType();
};

#endif // __TINYDB_AST_NODE_BASE_H__
