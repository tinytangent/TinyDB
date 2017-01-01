#ifndef __TINYDB_PRIMARY_KEY_CONSTRAINT_H__
#define __TINYDB_PRIMARY_KEY_CONSTRAINT_H__

#include "Constraint.h"
#include "NotNullConstraint.h"
#include "UniqueConstraint.h"

class PrimaryKeyConstraint : public Constraint
{
public:
    const std::string tableName;
    const std::string columnName;
protected:
    UniqueConstraint uniqueConstraint;
    NotNullConstraint notNullConstraint;
public:
    PrimaryKeyConstraint(const std::string &tableName, const std::string &columnName);
    bool checkAddRecord(Table *table, std::vector<SQLValue*> sqlValues) override;
    bool checkUpdateRecord(Table *table, std::vector<SQLValue*> sqlValues) override;
    bool checkDeleteRecord(Table *table, char* buffer) override;
};

#endif
