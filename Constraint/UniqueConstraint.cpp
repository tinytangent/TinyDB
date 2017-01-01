#include "Database/Database.h"
#include "Index/Index.h"
#include "UniqueConstraint.h"

UniqueConstraint::UniqueConstraint(Database * const database, const std::string &tableName, const std::string &columnName)
    :database(database), tableName(tableName), columnName(columnName)
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

bool UniqueConstraint::initialize()
{
    for (auto pair : database->getAllIndexes())
    {
        auto index = pair.second;
        if (index->tableName == this->tableName && index->columnName == columnName)
        {
            return true;
        }
    }
    std::string indexName = "";
    for (int i = 0; ; i++)
    {
        indexName = "index" + std::to_string(i);
        if (database->getIndex(indexName) == nullptr)
            break;
    }
    database->createIndex(indexName, tableName, columnName);
    return true;
}
