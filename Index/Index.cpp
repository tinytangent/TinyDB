#include <boost/log/trivial.hpp>
#include "Database/Database.h"
#include "Database/Table.h"
#include "FieldTypes/FieldList.h"
#include "FieldTypes/FieldType.h"
#include "Storage/CachedStorageArea.h"
#include "BPlusTree.h"
#include "Index.h"

Index::Index(Database * database, const std::string & tableName, const std::string & columnName, const boost::filesystem::path & storageFileName)
    :database(database), tableName(tableName), columnName(columnName), storagePath(storageFileName)
{
    Table *table = database->getTable(tableName);
    if (table == nullptr)
    {
        BOOST_LOG_TRIVIAL(fatal) << tableName << " doesn't exist, or is not a directory.";
    }
    fieldType = table->fieldList->getField(columnName).fieldType;
    indexFieldSize = fieldType->getConstantLength();
    indexKeySize = indexFieldSize + INDEX_ADDRESS_SIZE;
    storageArea = new CachedStorageArea(storagePath.string(), 4 * 1024 * 1024, 8192);
    bPlusTree = new BPlusTree(storageArea, indexKeySize, 0, fieldType);
}

bool Index::initialize()
{
    bPlusTree->initialize();
    return true;
}

bool Index::onAddRecord(uint64_t recordAddress, char* recordBuffer)
{
    char* buffer = new char[indexKeySize];
    memcpy(buffer, recordBuffer, indexFieldSize);
    memcpy(buffer + indexFieldSize, (char*)&recordAddress, sizeof(uint64_t));
    bPlusTree->insert(buffer);
    delete[] buffer;
    return true;
}

bool Index::onDeleteRecord(uint64_t recordAddress, char* recordBuffer)
{
    char* buffer = new char[indexKeySize];
    if (recordBuffer != nullptr)
    {
        memcpy(buffer, recordBuffer, indexFieldSize);
    }
    memcpy(buffer + indexFieldSize, (char*)&recordAddress, sizeof(uint64_t));
    bPlusTree->Delete(buffer);
    delete[] buffer;
    return true;
}
