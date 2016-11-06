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
    Table *table = new Table(this, tableName, fixedStoragePath, variableStorageName);
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

Database::Database(const std::string& name)
{
    setName(name);
    isOpened = false;
}

void Database::setName(const std::string& name)
{
    //TODO: Check name so that it doesn't include weird characters.
    this->databaseName = name;
    this->rootDirectory = Config::getRootDirectory();
    this->rootDirectory /= name;
    this->configFilePath = rootDirectory / "metadata.tinydb.json";
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
    boost::property_tree::ptree databases = configTree.get_child("database");
    tables.clear();
    for (auto v : databases)
    {
        std::string tableName = v.first;
        boost::property_tree::ptree tableDetail = v.second;
        boost::filesystem::path fixedStoragePath = rootDirectory / tableDetail.get<std::string>("fixedStorage");
        boost::filesystem::path variableStoragePath = rootDirectory / tableDetail.get<std::string>("variableStorage");
        tables[tableName] = new Table(this, tableName, fixedStoragePath, variableStoragePath);
    }
    return true;
}

bool Database::saveConfigFile()
{
    boost::property_tree::ptree databases;
    boost::property_tree::ptree indexes;
    boost::property_tree::ptree configTree;
    for (auto pair : tables)
    {
        std::string tableName = pair.first;
        Table* table = pair.second;
        databases.put(pair.first + "." + "fixedStorage", table->getFixedStoragePath().filename().string());
        databases.put(pair.first + "." + "variableStorage", table->getVariableStoragePath().filename().string());
    }
    configTree.put("name", databaseName);
    configTree.put_child("indexes", indexes);
    configTree.put_child("database", databases);

    std::fstream fileStream;
    //TODO: Should backup the original core config file.
    fileStream.open(configFilePath.c_str(), std::ios::trunc | std::ios::out);
    if (!fileStream.is_open())
    {
        return false;
    }
    write_json(fileStream, configTree, false);
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
    //TODO: Not implemented.
    return false;
}

bool Database::open()
{
    return loadConfigFile();
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

bool Database::dropTable(const std::string& tableName)
{
    return tables[tableName]->drop();
}

bool Database::dropDatabase(const std::string & databaseName)
{
    //TODO: Not implemented.
    return false;
}
