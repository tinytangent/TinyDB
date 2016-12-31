#ifndef __TINYDB_QUERY_SQL_EXECUTOR_H__
#define __TINYDB_QUERY_SQL_EXECUTOR_H__

class DBMS;
class Table;

class SQLExecutor
{
protected:
    DBMS *dbms;
public:
    SQLExecutor(DBMS *dbms);
    Table* getTable(const std::string &tableName);
    void executeCreateIndex(std::string indexName, const std::string &tableName, const std::string &columnName);
};

#endif
