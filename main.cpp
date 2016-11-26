#include <iostream>
#include <string>
#include "Utils/Config.h"
#include "Database/Database.h"
#include "Database/Table.h"
#include "Storage/DiskStorageArea.h"
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include "Parser/SQLParser.h"
#include "Parser/ASTNodeBase.h"
#include "Parser/ASTNodes.h"
#include "FieldTypes/FieldList.h"
#include "FieldTypes/IntegerFieldType.h"
#include "FieldTypes/BigIntFieldType.h"
#include "FieldTypes/SmallIntFieldType.h"

int main(int argc, char* argv[])
{
    /*std::string fileName = "D:/data.raw";
    srand(time(0));
    int a = rand();
    int b = rand();
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::debug
    );
    DiskStorageArea storageArea(fileName);
    for (int i = 0; i < 100; i++)
    {
        std::cout << (int)storageArea[i * 1024 * 1024 * 10] << std::endl;
        //std::cout << (int)storageArea[i * 1024 * 1024 * 10] << std::endl;
    }
    for (int i = 0; i < 100; i++)
    {
        storageArea[i * 1024 * 1024 * 10] = i + 100;
    }*/
    if(argc != 2)
    {
        std::cout << "Usage:TinyDB <rootDirectory>" << std::endl;
        return -1;
    }
    std::string rootDirectory = argv[1];
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
    Database database("Happy");
    database.open();
    for(;;)
    {
        std::getline(std::cin, command);
        if (command == "exit")
        {
            std::cout << "Goodbye!" << std::endl;
            break;
        }
        ASTNodeBase *node = SQLParser::parse(command);
        if (node == NULL)
        {
            std::cout << "Syntax error" << std::endl;
            continue;
        }
        else
        {
            switch(node->getType())
            {
            case ASTNodeBase::NodeType::CREATE_DATABASE_STATEMENT:
                std::cout << "Create database" << std::endl;
                break;
            case ASTNodeBase::NodeType::DROP_DATABASE_STATEMENT:
                std::cout << "Drop database" << std::endl;
                break;
            case ASTNodeBase::NodeType::CREATE_TABLE_STATEMENT:
            {
                database.createTable((ASTCreateTableStmtNode*)node);
                std::cout << "Create Table!" << std::endl;
                break;
            }
            case ASTNodeBase::NodeType::INSERT_INTO_STATEMENT:
            {
                auto stmtNode = (ASTInsertIntoStmtNode*)node;
                Table *table = database.getTable(stmtNode->name);
                if (table == nullptr)
                {
                    std::cout << "Table " << stmtNode->name << " doesn't exist!" << std::endl;
                    break;
                }
                table->open();
                if (!table->addRecord(stmtNode->values))
                {
                    std::cout << "Record insertion failed!" << std::endl;
                }
                std::cout << "Insert Into!" << std::endl;
                break;
            }
            }
        }
    }
    return 0;
}
