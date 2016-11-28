#ifndef __TINYDB_DBMS_H__
#define __TINYDB_DBMS_H__

#include <string>
#include <vector>
#include <boost/filesystem.hpp>

class Database;

class DBMS
{
protected:
    boost::filesystem::path dbmsDirectory;
    Database *currentDatabase;
public:
    DBMS(std::string& dbmsDirectory);
    std::vector<std::string> getAllDatabases();
    bool useDatabase(const std::string& dbName);
    Database* getCurrentDatabase(const std::string& dbName);
    bool createDatabase(const std::string& dbName);
    bool dropDatabase(const std::string& dbName);
};

#endif
