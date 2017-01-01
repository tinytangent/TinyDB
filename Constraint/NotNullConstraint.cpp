#include "NotNullConstraint.h"

NotNullConstraint::NotNullConstraint(const std::string &tableName, const std::string &columnName)
    :tableName(tableName), columnName(columnName)
{
    
}

bool NotNullConstraint::checkAddRecord(Table *table, std::vector<SQLValue*> sqlValues)
{
    return true;
}

bool NotNullConstraint::checkUpdateRecord(Table *table, std::vector<SQLValue*> sqlValues)
{
    return true;
}

bool NotNullConstraint::checkDeleteRecord(Table *table, char* buffer)
{
    return true;
}
