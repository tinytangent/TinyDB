#include <iostream>
#include "Parser/ASTNodes.h"
#include "Constraint.h"
#include "NotNullConstraint.h"
#include "UniqueConstraint.h"
#include "PrimaryKeyConstraint.h"

Constraint* Constraint::createFromASTNode(Database* database, ASTFieldConstraintNode *node)
{
    switch(node->type)
    {
        case CONSTRAINT_NONE:
            std::cout << "None" << std::endl;
            break;
        case CONSTRAINT_NOT_NULL:
            return new NotNullConstraint(node->tableName, node->columnName);
            break;
        case CONSTRAINT_NULL:
            std::cout << "Not null" << std::endl;
            break;
        case CONSTRAINT_CHECK:
            std::cout << "Check" << std::endl;
            break;
        case CONSTRAINT_DEFAULT:
            std::cout << "default" << std::endl;
            break;
        case CONSTRAINT_UNIQUE:
            return new UniqueConstraint(database, node->tableName, node->columnName);
        case CONSTRAINT_PRIMARY_KEY:
            return new PrimaryKeyConstraint(database, node->tableName, node->columnName);
        case CONSTRAINT_REFERENCES:
            std::cout << "foreign key" << std::endl;
            break;
    }
    return nullptr;
}

bool Constraint::initialize()
{
    return true;
}
