#ifndef __TINYDB_TABLE_H__
#define __TINYDB_TABLE_H__

#include <boost/filesystem.hpp>
#include <string>

class Database;
class ASTNodeBase;
class AbstractStorageArea;
class AbstractFixedAllocator;
class AbstractDynamicAllocator;
class CachedStorageArea;
class ASTCreateTableStmtNode;
class FieldList;

class Table
{
protected:
    Database* database;
    std::string tableName;
    boost::filesystem::path fixedStoragePath;
    boost::filesystem::path variableStoragePath;
    CachedStorageArea *fixedStorageArea = nullptr;
    AbstractStorageArea *dynamicStorageArea = nullptr;
    AbstractFixedAllocator *fixedAllocator = nullptr;
    AbstractDynamicAllocator *dynamicAllocator = nullptr;
    FieldList *fieldList = nullptr;
public:
    boost::filesystem::path getFixedStoragePath();
    boost::filesystem::path getVariableStoragePath();
    Table(
        Database* database,
        const std::string& tableName,
        const boost::filesystem::path& fixedStorageFileName,
        const boost::filesystem::path& variableStoragePath
    );
    bool initialize(ASTCreateTableStmtNode *astNode);
    bool drop();
    bool open();
    bool close();
    bool addRecord(std::vector<ASTNodeBase*> fields);
    bool updateRecord(int index, std::vector<ASTNodeBase*> fields);
    int findRecord(const std::string key, ASTNodeBase *value);
    bool deleteRecord(int index);
};

#endif
