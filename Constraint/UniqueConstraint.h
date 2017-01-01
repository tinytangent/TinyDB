#ifndef __TINYDB_UNIQUE_CONSTRAINT_H__
#define __TINYDB_UNIQUE_CONSTRAINT_H__

#include "Constraint.h"

class UniqueConstraint : public Constraint
{
public:
    const std::string tableName;
    const std::string columnName;
public:
    UniqueConstraint(const std::string &tableName, const std::string &columnName);
    bool checkAddRecord(Table *table, std::vector<SQLValue*> sqlValues) override;
    bool checkUpdateRecord(Table *table, std::vector<SQLValue*> sqlValues) override;
    bool checkDeleteRecord(Table *table, char* buffer) override;
};

#endif
