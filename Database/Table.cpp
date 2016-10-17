#include "Table.h"

boost::filesystem::path Table::getFixedStoragePath()
{
    return fixedStoragePath;
}

boost::filesystem::path Table::getVariableStoragePath()
{
    return variableStoragePath;
}

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
}

bool Table::exist()
{
    return false;
}

bool Table::create()
{
    return false;
}

bool Table::drop()
{
    return false;
}

bool Table::open()
{
    //TODO: not implemented.
    return false;
}

bool Table::close()
{
    //TODO: not implemented.
    return false;
}
