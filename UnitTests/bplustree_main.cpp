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
void test_bpt(int argc, char* argv[])
{
    srand(124);
    std::string fileName = "D:/data.raw";
    //DiskStorageArea storageArea(fileName);
    CachedStorageArea storageArea(fileName, 16 * 1024 * 1024, 8192);
    BlockAllocator blockAllocator(&storageArea);
    blockAllocator.initialize();
    uint64_t block = blockAllocator.allocate().getOffset();
    BPlusTree bpt(&storageArea, 4, 4);
    BPlusTree::Node rootNode(&bpt, block);
    rootNode.initialize();
    bpt.root = rootNode;
    const int TEST_DATA_COUNT = 500000;
    const int TEST_COUNT = TEST_DATA_COUNT / 2;
    int* arr = new int[TEST_DATA_COUNT];
    for (int i = 0; i < TEST_DATA_COUNT; i++)
    {
        arr[i] = i;
    }
    for (int i = 0; i < TEST_DATA_COUNT * 8; i++)
    {
        int p = (rand() * rand()) % TEST_DATA_COUNT;
        int q = (rand() * rand()) % TEST_DATA_COUNT;
        if (p != q) std::swap(arr[p], arr[q]);
    }
    std::cout << "Data generated." << std::endl;
    std::cout << "Testing insert 1." << std::endl;
    for (int i = 0; i < TEST_COUNT; i++)
    {
        bpt.insert(arr[i]);
    }
    for (int i = 0; i < TEST_COUNT; i++)
    {
        int key = arr[i];
        if (bpt.search(key) == 0 && i < TEST_COUNT)
        {
            std::cout << "Error" << std::endl;
        }
        else if (bpt.search(key) == 1 && i >= TEST_COUNT)
        {
            std::cout << "Error" << std::endl;
        }
    }
    std::cout << "Testing insert 2." << std::endl;
    for (int i = TEST_COUNT; i < TEST_DATA_COUNT; i++)
    {
        bpt.insert(arr[i]);
    }
    for (int i = 0; i < TEST_DATA_COUNT; i++)
    {
        int key = arr[i];
        if (bpt.search(key) == 0)
        {
            std::cout << "Error" << std::endl;
        }
    }
    std::cout << "Testing delete 1." << std::endl;
    for (int i = 0; i < TEST_COUNT; i++)
    {
        bpt.Delete(arr[i]);
    }
    for (int i = 0; i < TEST_COUNT; i++)
    {
        int key = arr[i];
        if (bpt.search(key) == 1 && i < TEST_COUNT)
        {
            std::cout << "Error1" << std::endl;
        }
        else if (bpt.search(key) == 0 && i >= TEST_COUNT)
        {
            std::cout << "Error2" << std::endl;
        }
    }
    std::cout << "Testing delete 2." << std::endl;
    for (int i = TEST_COUNT; i < TEST_DATA_COUNT; i++)
    {
        bpt.Delete(arr[i]);
    }
    for (int i = 0; i < TEST_DATA_COUNT; i++)
    {
        int key = arr[i];
        if (bpt.search(key) == 1)
        {
            std::cout << "Error1" << std::endl;
        }
    }
    if (bpt.root.getIsLeaf() && bpt.root.getUsedKeyCount() == 0)
    {
        std::cout << "OK." << std::endl;
    }
    else
    {
        std::cout << "Error." << std::endl;
    }
}
