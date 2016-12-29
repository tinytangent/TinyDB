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
    CachedStorageArea *dynamicStorageArea = nullptr;
    RecordAllocator *fixedAllocator = nullptr;
    AbstractDynamicAllocator *dynamicAllocator = nullptr;
    FieldList *fieldList = nullptr;
protected:
    bool addBinaryRecord(char* buffer);
public:
    std::vector<uint64_t> findBinaryRecordInRange(FieldType* fieldType, int fieldOffset, char* rangeMin, char* rangeMax, int mode);
    std::vector<uint64_t> findBinaryRecordByTableScan(const ASTExpression *expression);
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
    bool updateRecord(const std::string& fieldName,
        const ASTExpression const* expression,
        const ASTExpression const* whereExpression);
    std::vector<uint64_t> findRecord(const ASTExpression *expression);
    bool deleteRecord(ASTExpression *expression);
};

#endif
