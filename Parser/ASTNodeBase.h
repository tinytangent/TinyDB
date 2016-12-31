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
        TYPE_CHARACTER,
        DATA_VALUE,
        FIELD_CONSTRAINT,
        EXPRESSION,
        CREATE_TABLE_FIELD,
        CREATE_DATABASE_STATEMENT,
        DROP_DATABASE_STATEMENT,
        SHOW_DATABASES_STATEMENT,
        USE_DATABASE_STATEMENT,
        CREATE_TABLE_STATEMENT,
        INSERT_INTO_STATEMENT,
        SELECT_STATEMENT,
        SELECT_COLUMN_MAP,
        UPDATE_STATEMENT,
        DELETE_STATEMENT,
        DROP_TABLE_STATEMENT,
        SHOW_TABLES_STATEMENT,
        CREATE_INDEX_STATEMENT,
    };
protected:
    NodeType type;
    ASTNodeBase(NodeType type);
public:
    virtual NodeType getType();
};

#endif // __TINYDB_AST_NODE_BASE_H__
