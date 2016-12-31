#include <iostream>
#include "Database/DBMS.h"
#include "Database/Database.h"
#include "Database/Table.h"
#include "SQLExecutor.h"

SQLExecutor::SQLExecutor(DBMS *dbms)
    :dbms(dbms)
{

}

Table* SQLExecutor::getTable(const std::string &tableName)
{
    if (dbms->getCurrentDatabase() == nullptr)
    {
        std::cout << "Please select database first." << std::endl;
        return nullptr;
    }
    auto database = dbms->getCurrentDatabase();
    Table *table = database->getTable(tableName);
    if(table == nullptr)
    {
        std::cout << "Table " << tableName << " does not exist." << std::endl;
        return nullptr;
    }
    else if(!table->getIsOpened())
    {
        table->open();
    }
    return table;
}

void SQLExecutor::executeCreateIndex(std::string indexName, const std::string &tableName, const std::string &columnName)
{
    Table* table = getTable(tableName);
    if(table == nullptr) return;
    table->database->createIndex(indexName, tableName, columnName);
}
