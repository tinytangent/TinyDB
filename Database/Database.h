#ifndef __TINYDB_DATABASE_H__
#define __TINYDB_DATABASE_H__

#include <string>
#include <map>
#include <boost/filesystem.hpp>

class CachedStorageArea;
class AbstractDynamicAllocator;
class Table;

class Database
{
protected:
    std::string databaseName;
    boost::filesystem::path rootDirectory;
    boost::filesystem::path configFilePath;
    std::map<std::string, Table*> tables;
    bool isOpened;
protected:
    bool loadConfigFile();
    bool saveConfigFile();
public:
    Database(const std::string& name);
    void setName(const std::string& name);
    std::string getName();
    bool exist();
    bool create();
    bool drop();
    bool open();
    bool close();
    bool createTable(const std::string& tableName);
    bool dropDatabase(const std::string& tableName);
};

#endif
