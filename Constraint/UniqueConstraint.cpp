#include "UniqueConstraint.h"

UniqueConstraint::UniqueConstraint(const std::string &tableName, const std::string &columnName)
    :tableName(tableName), columnName(columnName)
{

}

bool UniqueConstraint::checkAddRecord(Table *table, std::vector<SQLValue*> sqlValues)
{
    return true;
}

bool UniqueConstraint::checkUpdateRecord(Table *table, std::vector<SQLValue*> sqlValues)
{
    return true;
}

bool UniqueConstraint::checkDeleteRecord(Table *table, char* buffer)
{
    return true;
}
