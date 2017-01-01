#ifndef __TINYDB_CONSTRAINT_H__
#define __TINYDB_CONSTRAINT_H__

#include <vector>

class SQLValue;
class Table;
class ASTFieldConstraintNode;

class Constraint
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
public:
    static Constraint* createFromASTNode(ASTFieldConstraintNode *node);
    //static bool createFromPTree();
    //static bool saveToPTree();
public:
    virtual bool checkAddRecord(Table *table, std::vector<SQLValue*> sqlValues) = 0;
    virtual bool checkUpdateRecord(Table *table, std::vector<SQLValue*> sqlValues) = 0;
    virtual bool checkDeleteRecord(Table *table, char* buffer) = 0;
};

#endif
