#include <iostream>
#include <string>
#include <fstream>
#include <boost/algorithm/string/predicate.hpp>
#include "Utils/Config.h"
#include "Database/DBMS.h"
#include "Database/Database.h"
#include "Database/Table.h"
#include "Storage/DiskStorageArea.h"
#include "Storage/BlockAllocator.h"
#include "Storage/CachedStorageArea.h"
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include "Parser/SQLParser.h"
#include "Parser/ASTNodeBase.h"
#include "Parser/ASTNodes.h"
#include "Expression/SuffixExpression.h"
#include "FieldTypes/FieldList.h"
#include "FieldTypes/IntegerFieldType.h"
#include "FieldTypes/BigIntFieldType.h"
#include "FieldTypes/SmallIntFieldType.h"
#include "FieldTypes/CharacterFieldType.h"
#include "Index/BPlusTree.h"

int main(int argc, char* argv[])
{
    if(argc <= 1)
    {
        std::cout << "Usage:TinyDB <rootDirectory> [sqlfile]" << std::endl;
        return -1;
    }
    std::string rootDirectory = argv[1];
    auto dbms = new DBMS(rootDirectory);
    std::cout << "Welcome to TinyDB!" << std::endl;
    Config::setRootDirectory(rootDirectory);
    if(!Config::startUpCheck())
    {
        return -1;
    }
    std::cout << "DBMS root directory : " << Config::getRootDirectory()
        << std::endl;
    std::string command;
    FieldType::registerType("integer", new IntegerFieldType());
    FieldType::registerType("bigint", new BigIntFieldType());
    FieldType::registerType("smallint", new SmallIntFieldType());
    FieldType::registerType("character", new CharacterFieldType());
    Database *database = nullptr;
    std::istream* inputStream = nullptr;
    for(;;)
    {
        if (inputStream == nullptr)
        {
            if (argc >= 3)
            {
                std::string fileName = argv[2];
                std::ifstream *file = new std::ifstream(fileName);
                if (!file->is_open()) return 0;
                std::cout << "Using file" << std::endl;
                inputStream = file;
                //return 0;
            }
            else
            {
                inputStream = &std::cin;
            }
        }
        std::vector<ASTNodeBase*> nodeVector;
        if (inputStream->eof()) break;
        const int LINE_LENGTH = 100;
        char str[LINE_LENGTH];
        inputStream->getline(str, 1000);
        command = str;
        //std::getline(*inputStream, command);
        if (inputStream->eof()) break;
        if (boost::starts_with(command, ".read "))
        {
            command = command.substr(6);
            std::ifstream inputFileStream(command);
            if (inputFileStream.is_open())
            {
                nodeVector = SQLParser::parseStream(inputFileStream);
                inputFileStream.close();
            }
            else
            {
                std::cout << "Cannot open " << command << std::endl;
            }
        }
        else if (command == "exit" || command == ".exit" || command == "quit" || command == ".quit")
        {
            std::cout << "Goodbye!" << std::endl;
            exit(0);
            break;
        }
        else
        {
            nodeVector = SQLParser::parse(command);
        }

        for(auto node : nodeVector)
        {
            switch(node->getType())
            {
            case ASTNodeBase::NodeType::CREATE_DATABASE_STATEMENT:
            {
                auto stmtNode = (ASTCreateDatabaseStmtNode*)node;
                auto dbName = stmtNode->dbName->name;
                if (!dbms->createDatabase(dbName))
                {
                    std::cout << "Failed to create database "
                        << dbName << std::endl;
                }
                else
                {
                    std::cout << "Database " << dbName
                        << " created" << std::endl;
                }
                break;
            }
            case ASTNodeBase::NodeType::DROP_DATABASE_STATEMENT:
            {
                auto stmtNode = (ASTDropDatabaseStmtNode*)node;
                auto dbName = stmtNode->dbName->name;
                if (!dbms->dropDatabase(dbName))
                {
                    std::cout << "Failed to drop database "
                        << dbName << std::endl;
                }
                else
                {
                    std::cout << "Database " << dbName
                        << " dropped" << std::endl;
                }
                break;
            }
            case ASTNodeBase::NodeType::SHOW_DATABASES_STATEMENT:
            {
                auto dbList = dbms->getAllDatabases();
                for (auto entry : dbList)
                {
                    std::cout << entry << std::endl;
                }
                break;
            }
            case ASTNodeBase::NodeType::USE_DATABASE_STATEMENT:
            {
                auto stmtNode = (ASTUseDatabaseStmtNode*)node;
                auto dbName = stmtNode->dbName->name;
                if (!dbms->useDatabase(dbName))
                {
                    std::cout << "Failed to switch database "
                        << dbName << std::endl;
                }
                else
                {
                    std::cout << "Switched to database "
                        << dbName << std::endl;
                }
                break;
            }
            case ASTNodeBase::NodeType::CREATE_TABLE_STATEMENT:
            {
                if (dbms->getCurrentDatabase() == nullptr)
                {
                    std::cout << "Please select database first." << std::endl;
                    break;
                }
                database = dbms->getCurrentDatabase();
                database->createTable((ASTCreateTableStmtNode*)node);
                std::cout << "Create Table!" << std::endl;
                break;
            }
            case ASTNodeBase::NodeType::INSERT_INTO_STATEMENT:
            {
                if (dbms->getCurrentDatabase() == nullptr)
                {
                    std::cout << "Please select database first." << std::endl;
                    break;
                }
                database = dbms->getCurrentDatabase();
                auto stmtNode = (ASTInsertIntoStmtNode*)node;
                Table *table = database->getTable(stmtNode->name);
                if (table == nullptr)
                {
                    std::cout << "Table " << stmtNode->name << " doesn't exist!" << std::endl;
                    break;
                }
                table->open();
                for (auto &tuple : stmtNode->values)
                {
                    if (!table->addRecord(tuple))
                    {
                        std::cout << "Record insertion failed!" << std::endl;
                    }
                }
                std::cout << "Insert Into!" << std::endl;
                //table->close();
                break;
            }
            case ASTNodeBase::NodeType::SELECT_STATEMENT:
            {
                if (dbms->getCurrentDatabase() == nullptr)
                {
                    std::cout << "Please select database first." << std::endl;
                    break;
                }
                database = dbms->getCurrentDatabase();
                auto stmtNode = (ASTSelectStmtNode*)node;
                Table *table = database->getTable(stmtNode->tableName);
                table->open();
                table->findRecord(stmtNode->expression);
                std::cout << "Select!" << std::endl;
                break;
            }
            case ASTNodeBase::NodeType::UPDATE_STATEMENT:
            {
                if (dbms->getCurrentDatabase() == nullptr)
                {
                    std::cout << "Please select database first." << std::endl;
                    break;
                }
                database = dbms->getCurrentDatabase();
                auto stmtNode = (ASTUpdateStmtNode*)node;
                Table *table = database->getTable(stmtNode->tableName);
                table->open();
                table->updateRecord(stmtNode->columnName,
                    stmtNode->updateExpression, stmtNode->whereExpression);
                std::cout << "Update!" << std::endl;
                break;
            }
            case ASTNodeBase::NodeType::DELETE_STATEMENT:
            {
                if (dbms->getCurrentDatabase() == nullptr)
                {
                    std::cout << "Please select database first." << std::endl;
                    break;
                }
                database = dbms->getCurrentDatabase();
                auto stmtNode = (ASTSelectStmtNode*)node;
                Table *table = database->getTable(stmtNode->tableName);
                table->open();
                table->deleteRecord(stmtNode->expression);
                std::cout << "Delete!" << std::endl;
                break;
            }
            case ASTNodeBase::NodeType::SHOW_TABLES_STATEMENT:
            {
                if (dbms->getCurrentDatabase() == nullptr)
                {
                    std::cout << "Please select database first." << std::endl;
                    break;
                }
                database = dbms->getCurrentDatabase();
                for (auto i : database->getAllTables())
                {
                    std::cout << i.first << std::endl;
                }
                break;
            }
            case ASTNodeBase::NodeType::DROP_TABLE_STATEMENT:
            {
                auto stmtNode = (ASTDropTableStmtNode*)node;
                if (dbms->getCurrentDatabase() == nullptr)
                {
                    std::cout << "Please select database first." << std::endl;
                    break;
                }
                database = dbms->getCurrentDatabase();
                auto tableName = stmtNode->tableName;
                if (!database->dropTable(tableName))
                {
                    std::cout << "Failed to drop table "
                        << tableName << std::endl;
                }
                else
                {
                    std::cout << "Droped table "
                        << tableName << std::endl;
                }
                break;
            }
            }
        }
    }
    return 0;
}
