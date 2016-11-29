#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include "Database.h"
#include "DBMS.h"

DBMS::DBMS(std::string& dbmsDirectory)
    :dbmsDirectory(dbmsDirectory)
{
    this->currentDatabase = nullptr;
}

std::vector<std::string> DBMS::getAllDatabases()
{
    std::vector<std::string> ret;
    for (auto& entry : boost::make_iterator_range(boost::filesystem::directory_iterator(dbmsDirectory), {}))
    {
        const boost::filesystem::path &path = entry;
        ret.push_back(path.filename().string());
    }
    return ret;
}

bool DBMS::useDatabase(const std::string & dbName)
{
    Database *database = new Database(dbName, dbmsDirectory / dbName);
    bool ret = false;
    if (!database->exist())
    {
        std::cout << "Cannot switch to non-existing database " <<
            dbName << std::endl;
        ret = false;
    }
    else
    {
        ret = database->open();
    }
    if (!ret)
    {
        delete database;
    }
    else
    {
        currentDatabase = database;
    }
    return ret;
}

Database * DBMS::getCurrentDatabase()
{
    return currentDatabase;
}

bool DBMS::createDatabase(const std::string & dbName)
{
    Database *database = new Database(dbName, dbmsDirectory / dbName);
    bool ret = false;
    if (database->exist())
    {
        std::cout << "Cannot create existing database " <<
            dbName << std::endl;
        ret = false;
    }
    else
    {
        ret = database->create();
    }
    delete database;
    return ret;
}

bool DBMS::dropDatabase(const std::string & dbName)
{
    Database *database = new Database(dbName, dbmsDirectory / dbName);
    bool ret = false;
    if (!database->exist())
    {
        std::cout << "Cannot drop non-existing database " <<
            dbName << std::endl;
        ret = false;
    }
    else
    {
        ret = database->drop();
    }
    delete database;
    return ret;
}
