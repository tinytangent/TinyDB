#ifndef __TINYDB_TABLE_H__
#define __TINYDB_TABLE_H__

#include <boost/filesystem.hpp>
#include <string>

class Database;
class AbstractStorageArea;
class AbstractFixedAllocator;
class AbstractDynamicAllocator;

class Table
{
protected:
    Database* database;
    std::string tableName;
    boost::filesystem::path fixedStoragePath;
    boost::filesystem::path variableStoragePath;
    AbstractFixedAllocator *fixedAllocator;
    AbstractDynamicAllocator *dynamicAllocator;
public:
    boost::filesystem::path getFixedStoragePath();
    boost::filesystem::path getVariableStoragePath();
    Table(
        Database* database,
        const std::string& tableName,
        const boost::filesystem::path& fixedStorageFileName,
        const boost::filesystem::path& variableStoragePath
    );
    bool exist();
    bool create();
    bool drop();
    bool open();
    bool close();
};

#endif
