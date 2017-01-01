#include "PrimaryKeyConstraint.h"

PrimaryKeyConstraint::PrimaryKeyConstraint(const std::string &tableName, const std::string &columnName)
    : notNullConstraint(tableName, columnName),
    uniqueConstraint(tableName, columnName)
{

}

bool PrimaryKeyConstraint::checkAddRecord(Table *table, std::vector<SQLValue*> sqlValues)
{
    return notNullConstraint.checkAddRecord(table, sqlValues) &&
        uniqueConstraint.checkAddRecord(table, sqlValues);
}

bool PrimaryKeyConstraint::checkUpdateRecord(Table *table, std::vector<SQLValue*> sqlValues)
{
    return notNullConstraint.checkUpdateRecord(table, sqlValues) &&
        uniqueConstraint.checkUpdateRecord(table, sqlValues);
}

bool PrimaryKeyConstraint::checkDeleteRecord(Table *table, char* buffer)
{
    return notNullConstraint.checkDeleteRecord(table, buffer) &&
        uniqueConstraint.checkDeleteRecord(table, buffer);
}