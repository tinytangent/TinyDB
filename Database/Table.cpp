#include "FieldTypes/FieldList.h"
#include "FieldTypes/FieldType.h"
#include "Storage/CachedStorageArea.h"
#include "Storage/AbstractStorageArea.h"
#include "Storage/AbstractFixedAllocator.h"
#include "Storage/FixedAllocator.h"
#include "Parser/ASTNodes.h"
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

bool Table::initialize(ASTCreateTableStmtNode *astNode)
{
    fixedStorageArea = new CachedStorageArea(fixedStoragePath.string(), 4 * 1024 * 1024, 8192);
    fieldList = FieldList::fromASTNode(astNode->fields);
    uint32_t bytesReserved = fieldList->getHeaderSize() + sizeof(uint32_t);
    bytesReserved = (bytesReserved / 4096 + 1) * 4096;
    fixedStorageArea->setDataAt(0, (char*)&bytesReserved, sizeof(uint32_t));
    fixedStorageArea->setDataAt(sizeof(uint32_t), fieldList->getHeaderData(), fieldList->getHeaderSize());
    fixedStorageArea->flush();
    //TODO: error handling.
    return true;
}

bool Table::drop()
{
    boost::filesystem::remove(fixedStoragePath);
    boost::filesystem::remove(variableStoragePath);
    return true;
}

bool Table::open()
{
    uint32_t bytesReserved;
    fixedStorageArea = new CachedStorageArea(fixedStoragePath.string(), 4 * 1024 * 1024, 8192);
    dynamicStorageArea = new CachedStorageArea(variableStoragePath.string(), 4 * 1024 * 1024, 8192);
    fixedAllocator = new FixedAllocator(fixedStorageArea, bytesReserved, 8192);
    fixedStorageArea->getDataAt(0, (char*)&bytesReserved, sizeof(uint32_t));
    //TDOO: dynamci storage area.
    //TODO: error handling.
    return true;
}

bool Table::close()
{
    delete fixedAllocator;
    delete dynamicStorageArea;
    delete fixedStorageArea;
    //TODO: error handling.
    return true;
}

bool Table::addRecord(std::vector<ASTNodeBase*> records)
{
    char* buffer = new char[fieldList->getRecordFixedSize()];
    for (int i = 0; i < fieldList->getCompiledFields().size(); i++)
    {
        auto field = fieldList->getCompiledFields()[i].fieldType;
        int fieldSize = field->getDataLength(records[i]);
        char* temp = new char[fieldSize];
        char* bufferPos = buffer;
        field->toBinary(records[i], temp);
        if (fieldSize + sizeof(uint32_t) > 32)
        {
            memcpy(bufferPos, (char*)&fieldSize, 4);
            //TODO: Add to variable storage area.
            memcpy(bufferPos, temp, 20);
            bufferPos += 32;
        }
        else
        {
            memcpy(bufferPos, temp, fieldSize);
            bufferPos += fieldSize;
        }
    }
    AbstractStorageArea::AccessProxy addr = fixedAllocator->allocate();
    fixedStorageArea->setDataAt(addr.getOffset(), buffer, fieldList->getRecordFixedSize());
    //TODO: error handling
    return true;
}

bool Table::deleteRecord(int index)
{
    fixedAllocator->free((*fixedStorageArea)[index * fieldList->getRecordFixedSize()]);
    //TODO: Handle variable storage area.
    return true;
}

int Table::findRecord(const std::string key, ASTNodeBase *value)
{
    char *buffer = new char[fieldList->getRecordFixedSize()];
    for (int index = 0; ; index++)
    {
        fixedStorageArea->getDataAt(index * fieldList->getRecordFixedSize(), buffer, fieldList->getRecordFixedSize());
        if (fieldList->getCompiledFields()[0].fieldType->compare(buffer, value) == 0)
            return index;
    }
    return -1;
    
}

bool Table::updateRecord(int index, std::vector<ASTNodeBase*> records)
{
    char* buffer = new char[fieldList->getRecordFixedSize()];
    for (int i = 0; i < fieldList->getCompiledFields().size(); i++)
    {
        auto field = fieldList->getCompiledFields()[i].fieldType;
        int fieldSize = field->getDataLength(records[i]);
        char* temp = new char[fieldSize];
        char* bufferPos = buffer;
        field->toBinary(records[i], temp);
        if (fieldSize + sizeof(uint32_t) > 32)
        {
            memcpy(bufferPos, (char*)&fieldSize, 4);
            //TODO: Add to variable storage area.
            memcpy(bufferPos, temp, 20);
            bufferPos += 32;
        }
        else
        {
            memcpy(bufferPos, temp, fieldSize);
            bufferPos += fieldSize;
        }
    }
    fixedStorageArea->setDataAt(index * fieldList->getRecordFixedSize(), buffer, fieldList->getRecordFixedSize());
    //TODO: error handling
    return true;
}