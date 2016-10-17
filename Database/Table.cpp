#include "FieldTypes/FieldList.h"
#include "Storage/AbstractStorageArea.h"
#include "Table.h"

Table::Table(
    Database *database,
    const std::string& tableName,
    const boost::filesystem::path& fixedStorageFileName,
    const boost::filesystem::path& variableStorageFileName
) {
    this->database = database;
    //this->dynamicAllocator = dynamicAllocator;
    this->tableName = tableName;
    this->fixedStoragePath = fixedStorageFileName;
    this->variableStoragePath = variableStorageFileName;
    //TODO: initialize the storage area.
}

boost::filesystem::path Table::getFixedStoragePath()
{
    return fixedStoragePath;
}

boost::filesystem::path Table::getVariableStoragePath()
{
    return variableStoragePath;
}

bool Table::initialize(ASTNodeBase *astNodeBase)
{
    fieldList = new FieldList();
    //TODO: Add fields to fieldList by using astNodeBase.
    fieldList->compile();
    uint32_t bytesReserved = fieldList->getHeaderSize() + sizeof(uint32_t);
    bytesReserved = (bytesReserved / 4096 + 1) * 4096;
    fixedStorageArea->setDataAt(0, (char*)&bytesReserved, sizeof(uint32_t));
    fixedStorageArea->setDataAt(sizeof(uint32_t), fieldList->getHeaderData(), fieldList->getHeaderSize());
    //TODO: error handling.
    return true;
}

bool Table::drop()
{
    return false;
}

bool Table::open()
{
    uint32_t bytesReserved;
    fixedStorageArea->getDataAt(0, (char*)&bytesReserved, sizeof(uint32_t));
    //TODO: initialize allocators.
    //TODO: error handling.
    return false;
}

bool Table::close()
{
    //TODO: not implemented.
    return false;
}
