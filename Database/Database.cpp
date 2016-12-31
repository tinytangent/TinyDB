#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "Utils/Config.h"
#include "Table.h"
#include "Database.h"
#include "Parser/ASTNodes.h"
#include "Index/Index.h"

bool Database::createTableFile(const std::string & tableName)
{
    std::string fixedStorageName = boost::uuids::to_string(boost::uuids::random_generator()());
    std::string variableStorageName = boost::uuids::to_string(boost::uuids::random_generator()());
    fixedStorageName = fixedStorageName.substr(1, fixedStorageName.size() - 2);
    variableStorageName = variableStorageName.substr(1, fixedStorageName.size() - 2);
    auto fixedStoragePath = rootDirectory / fixedStorageName;
    auto variableStoragePath = rootDirectory / variableStorageName;
    std::ofstream fileStream;
    fileStream.open(fixedStoragePath.c_str(), std::ios::out | std::ios::trunc);
    if (!fileStream.is_open())
    {
        BOOST_LOG_TRIVIAL(error) <<
            "Cannot create database file " << fixedStoragePath;
        return false;
    }
    fileStream.close();
    fileStream.open(variableStoragePath.c_str(), std::ios::out | std::ios::trunc);
    if (!fileStream.is_open())
    {
        BOOST_LOG_TRIVIAL(error) <<
            "Cannot create database file " << variableStoragePath;
        return false;
    }
    fileStream.close();
    Table *table = new Table(this, tableName, fixedStoragePath, variableStoragePath);
    tables[tableName] = table;
    return saveConfigFile();
}

bool Database::deleteTableFile(const std::string & tableName)
{
    if (tables.find(tableName) == tables.end())
    {
        return false;
    }
    Table *table = tables[tableName];
    table->close();
    boost::filesystem::remove(table->getFixedStoragePath());
    boost::filesystem::remove(table->getVariableStoragePath());
    delete table;
    tables.erase(tableName);
    return saveConfigFile();
}

Database::Database(const std::string& name, const boost::filesystem::path& path)
{
    this->databaseName = name;
    this->rootDirectory = path;
    this->configFilePath = rootDirectory / "metadata.tinydb.json";
    isOpened = false;
}

void Database::setName(const std::string& name)
{
    //TODO: Check name so that it doesn't include weird characters.

}

std::string Database::getName()
{
    return this->databaseName;
}

bool Database::loadConfigFile()
{
    if(isOpened)
    {
        BOOST_LOG_TRIVIAL(error) <<
            "Trying to load config file when database" << databaseName <<
            "is opened!";
        return false;
    }
    boost::property_tree::ptree configTree;
    std::ifstream fileStream;
    fileStream.open(configFilePath.c_str());
    if(!fileStream.is_open())
    {
        return false;
    }
    read_json(fileStream, configTree);
    fileStream.close();
    boost::property_tree::ptree indexes = configTree.get_child("indexes");
    boost::property_tree::ptree databases = configTree.get_child("tables");
    tables.clear();
    for (auto v : databases)
    {
        std::string tableName = v.first;
        boost::property_tree::ptree tableDetail = v.second;
        boost::filesystem::path fixedStoragePath = rootDirectory / tableDetail.get<std::string>("fixedStorage");
        boost::filesystem::path variableStoragePath = rootDirectory / tableDetail.get<std::string>("variableStorage");
        tables[tableName] = new Table(this, tableName, fixedStoragePath, variableStoragePath);
    }
    for (auto v : indexes)
    {
        std::string indexName = v.first;
        boost::property_tree::ptree indexDetail = v.second;
        std::string tableName = indexDetail.get<std::string>("tableName");
        std::string columnName = indexDetail.get<std::string>("columnName");
        boost::filesystem::path storagePath = rootDirectory / indexDetail.get<std::string>("storageArea");
        tables[tableName] = new Table(this, tableName, columnName, storagePath);
    }
    return true;
}

bool Database::saveConfigFile()
{
    boost::property_tree::ptree databases;
    boost::property_tree::ptree indexes;
    boost::property_tree::ptree constraints;
    boost::property_tree::ptree configTree;
    for (auto pair : tables)
    {
        std::string tableName = pair.first;
        Table* table = pair.second;
        databases.put(pair.first + "." + "storageArea", table->getFixedStoragePath().filename().string());
        databases.put(pair.first + "." + "variableStorage", table->getVariableStoragePath().filename().string());
    }
    for (auto pair : this->indexes)
    {
        std::string indexName = pair.first;
        Index* index = pair.second;
        indexes.put(pair.first + "." + "tableName", index->tableName);
        indexes.put(pair.first + "." + "columnName", index->columnName);
        indexes.put(pair.first + "." + "storageArea", index->storagePath.filename().string());
    }
    configTree.put("name", databaseName);
    configTree.put_child("indexes", indexes);
    configTree.put_child("tables", databases);
    configTree.put_child("constraints", constraints);

    std::fstream fileStream;
    //TODO: Should backup the original core config file.
    fileStream.open(configFilePath.c_str(), std::ios::trunc | std::ios::out);
    if (!fileStream.is_open())
    {
        return false;
    }
    write_json(fileStream, configTree, true);
    fileStream.close();
    return true;
}

bool Database::exist()
{
    return
        boost::filesystem::is_directory(rootDirectory) &&
        boost::filesystem::is_regular_file(configFilePath);
}

bool Database::create()
{
    if(exist())
    {
        BOOST_LOG_TRIVIAL(error) <<
            "Trying to create existing database " << databaseName;
        return false;
    }

    if(!boost::filesystem::is_directory(rootDirectory) &&
        !boost::filesystem::create_directory(rootDirectory))
    {
        BOOST_LOG_TRIVIAL(error) <<
            "Cannot create directory " << rootDirectory;
        return false;
    }
    saveConfigFile();
    return true;
}

bool Database::drop()
{
    if(!exist())
    {
        BOOST_LOG_TRIVIAL(error) <<
            "Trying to delete non-existing database " << databaseName;
        return false;
    }
    boost::system::error_code errc;
    boost::filesystem::remove_all(rootDirectory, errc);
    return errc == boost::system::errc::success;
}

bool Database::open()
{
    if(!loadConfigFile())
    {
        BOOST_LOG_TRIVIAL(error) <<
            "Cannot load config file for database " << databaseName;
        return false;
    }
    for (auto i : tables)
    {
        if (!i.second->open())
        {
            BOOST_LOG_TRIVIAL(error) <<
                "Cannot open database " << i.first;
            return false;
        }
    }
    return true;
}

bool Database::close()
{
    return false;
}

bool Database::createTable(ASTCreateTableStmtNode* astNode)
{
    if (!createTableFile(astNode->name))
    {
        //TODO : Delete related files;
        return false;
    }
    Table *table = tables[astNode->name];
    if (!table->initialize(astNode))
    {
        //TODO : Delete related files;
        return false;
    }
    return true;
}

bool Database::createIndex(const std::string& indexName, const std::string& tableName, const std::string& columnName)
{
    std::string storageName = boost::uuids::to_string(boost::uuids::random_generator()());
    auto storagePath = rootDirectory / storageName;
    Index * index = new Index(this, tableName, columnName, storagePath);
    indexes[indexName] = index;
    //TODO: Error handling.
    return saveConfigFile();
}

Table * Database::getTable(const std::string & tableName)
{
    if (tables.find(tableName) == tables.end())
    {
        return nullptr;
    }
    return tables[tableName];
}

Index * Database::getIndex(const std::string & indexName)
{
    if (indexes.find(indexName) == indexes.end())
    {
        return nullptr;
    }
    return indexes[indexName];
}

std::map<std::string, Table*>& Database::getAllTables()
{
    return tables;
}

std::map<std::string, Index*>& Database::getAllIndexes()
{
    return indexes;
}

bool Database::dropTable(const std::string& tableName)
{
    if (!tables[tableName]->drop())
    {
        return false;
    }
    tables.erase(tableName);
    return saveConfigFile();
}
