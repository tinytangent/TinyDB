#ifndef __TINYDB_UNIQUE_CONSTRAINT_H__
#define __TINYDB_UNIQUE_CONSTRAINT_H__

#include "Constraint.h"

class Database;

class UniqueConstraint : public Constraint
{
public:
    const std::string tableName;
    const std::string columnName;
    Database * const database;
public:
    UniqueConstraint(Database * const database, const std::string &tableName, const std::string &columnName);
    bool checkAddRecord(Table *table, std::vector<SQLValue*> sqlValues) override;
    bool checkUpdateRecord(Table *table, std::vector<SQLValue*> sqlValues) override;
    bool checkDeleteRecord(Table *table, char* buffer) override;
    bool initialize() override;
};

#endif
