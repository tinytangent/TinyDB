#include <iostream>
#include <string>
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
	srand(124);
	std::string fileName = "D:/data.raw";
	//DiskStorageArea storageArea(fileName);
	CachedStorageArea storageArea(fileName, 8 * 1024 * 1024, 8192);
	BlockAllocator blockAllocator(&storageArea);
	blockAllocator.initialize();
	uint64_t block = blockAllocator.allocate().getOffset();
	BPlusTree bpt(&storageArea, 4, 4);
	BPlusTree::Node rootNode(&bpt, block);
	rootNode.initialize();
	bpt.root = rootNode;
	int arr[1024];
	for (int i = 0; i < 1024; i++)
	{
		arr[i] = i;
	}
	for (int i = 0; i < 1024 * 8; i++)
	{
		int p = rand() % 1024;
		int q = rand() % 1024;
		if (p != q) std::swap(arr[p], arr[q]);
	}
	const int TEST_COUNT = 1024 / 2;
	for (int i = 0; i < TEST_COUNT; i++)
	{
		if (i % 100 == 0) std::cout << i << std::endl;
		bpt.insert(arr[i]);
	}
	/*for (int i = 0; i < 1024; i++)
	{
		int Search(int x, BPlusTree::Node *root);
		//std::cout << i << std::endl;
		if (Search(arr[i], &bpt.root) == 1 && i >= TEST_COUNT)
		{
			std::cout << "Error " << i << " " << arr[i] << std::endl;
		}
		else if (Search(arr[i], &bpt.root) == 0 && i < TEST_COUNT)
		{
			std::cout << "Error " << i << " " << arr[i] << std::endl;
		}
	}*/
	std::cout << "Done." << std::endl;
	for (int i = 0; i < 4000; i++)
	{
		//std::cout << i << ":"<<arr[i]<<std::endl;
		bpt.Delete(arr[i]);
		int Search(int x, BPlusTree::Node *root);
		if (Search(arr[i], &bpt.root) == 1)
		{
			std::cout << "Error " << i << " " << arr[i] << std::endl;
		}
	}
	for (int i = 4000; i < 1024; i++)
	{
		int Search(int x, BPlusTree::Node *root);
		//std::cout << i << std::endl;
		if (Search(arr[i], &bpt.root) == 1 && i >= TEST_COUNT)
		{
			std::cout << "Error " << i << " " << arr[i] << std::endl;
		}
		else if (Search(arr[i], &bpt.root) == 0 && i < TEST_COUNT)
		{
			std::cout << "Error " << i << " " << arr[i] << std::endl;
		}
	}
	std::cout << bpt.root.address << std::endl;
	system("pause"); system("pause");
	return 0;
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
    }
    /*if(argc != 2)
    {
        std::cout << "Usage:TinyDB <rootDirectory>" << std::endl;
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
                if (!table->addRecord(stmtNode->values))
                {
                    std::cout << "Record insertion failed!" << std::endl;
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
    return 0;*/
}
