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
    srand(123);
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
    int arr[65536];
    for (int i = 0; i < 65536; i++)
    {
        arr[i] = i;
    }
    for (int i = 0; i < 65536 * 8; i++)
    {
        int p = rand() % 65536;
        int q = rand() % 65536;
        if (p != q) std::swap(arr[p], arr[q]);
    }
    const int TEST_COUNT = 65536 / 2;
    for (int i = 0; i < TEST_COUNT; i++)
    {
        if (i % 100 == 0) std::cout << i << std::endl;
        bpt.insert(arr[i], &bpt.root);
    }
    for (int i = 0; i < 65536; i++)
    {
        int Search(int x, BPlusTree::Node *root);
        //std::cout << i << std::endl;
        if (Search(arr[i], &bpt.root) == 1 && i >= TEST_COUNT)
        {
            std::cout << "Error " << i << " " << arr[i] << std::endl;
        }
        else if(Search(arr[i], &bpt.root) == 0 && i < TEST_COUNT)
        {
            std::cout << "Error " << i << " " << arr[i] << std::endl;
        }
    }
    std::cout << "Done." << std::endl;
    std::cout << bpt.root.address << std::endl;
    return 0;
}
