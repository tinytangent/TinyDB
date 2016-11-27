#ifndef __TINYDB_TABLE_H__
#define __TINYDB_TABLE_H__

#include <string>
#include <list>
#include <boost/filesystem.hpp>

class Database;
class ASTNodeBase;
class AbstractStorageArea;
class AbstractFixedAllocator;
class RecordAllocator;
class AbstractDynamicAllocator;
class CachedStorageArea;
class ASTCreateTableStmtNode;
class FieldList;
class FieldType;
class ASTSQLDataValue;
class ASTExpression;

class Table
{
protected:
    bool isOpened = false;
protected:
    Database* database;
    std::string tableName;
    boost::filesystem::path fixedStoragePath;
    boost::filesystem::path variableStoragePath;
    CachedStorageArea *fixedStorageArea = nullptr;
    AbstractStorageArea *dynamicStorageArea = nullptr;
    RecordAllocator *fixedAllocator = nullptr;
    AbstractDynamicAllocator *dynamicAllocator = nullptr;
    FieldList *fieldList = nullptr;
protected:
    bool addBinaryRecord(char* buffer);
public:
    void findBinaryRecordInRange(FieldType* fieldType, int fieldOffset, char* rangeMin, char* rangeMax, int mode);
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
    bool addRecord(std::list<ASTSQLDataValue*> fields);
    bool updateRecord(int index, std::vector<ASTNodeBase*> fields);
    int findRecord(ASTExpression *expression);
    bool deleteRecord(int index);
};

#endif
