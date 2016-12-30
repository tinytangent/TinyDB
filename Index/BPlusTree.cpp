#include <cassert>
#include <iostream>
#include <string>
#include "../Storage/AbstractStorageArea.h"
#include "BPlusTree.h"
#include "../Storage/BlockAllocator.h"
using namespace std;
const int Max_Number_Of_Branches = 100;

BPlusTree::BPlusTree(AbstractStorageArea* storageArea, int keySize, int valueSize)
    :root(this, 0)
{
    this->storageArea = storageArea;
    allocator = new BlockAllocator(storageArea);
    this->keySize = keySize;
    this->valueSize = valueSize;
    this->maxDataPerNode =
        8192 / (this->keySize + this->valueSize + sizeof(uint64_t)) - 1;
    this->pageSize = 8192; // TODO : Get from storage area may be a better idea?
    nodeParentOffset = 0;
    nodeNextOffset = nodeParentOffset + 8;
    nodeUsedCountOffset = nodeNextOffset + 8;
    nodeIsLeafOffset = nodeUsedCountOffset + 4;
    nodeKeysOffset = nodeIsLeafOffset + 4;
    nodeBranchesOffset = nodeKeysOffset + keySize * (maxDataPerNode - 1);
    nodeBranchDataOffset = nodeBranchesOffset + sizeof(uint64_t) * maxDataPerNode;
}

BPlusTree::Node BPlusTree::splitLeafNode(BPlusTree::Node fullNode)
{
	//std::cout << "split";
    char *buffer = new char[keySize];
    assert(fullNode.getUsedKeyCount() == Max_Number_Of_Branches);
    BPlusTree::Node parent = fullNode.getParent();
    if (parent.isNull())
    {
        parent = allocateNode();
        parent.setIsLeaf(false);
        root = parent;
        fullNode.setParent(parent);
    }
    int splitLeft = (fullNode.getUsedKeyCount() - 1) / 2;
    int splitRight = splitLeft + 1;
    BPlusTree::Node newNode = allocateNode();
    newNode.setParent(parent);
    int leftSize = splitLeft + 1;
    int rightSize = Max_Number_Of_Branches - leftSize;

    newNode.setIsLeaf(fullNode.getIsLeaf());
    newNode.setUsedKeyCount(rightSize);
    for (int i = 0; i < rightSize; i++)
    {
        fullNode.getKey(splitRight + i, buffer);
        newNode.setKey(i, buffer);
    }

    fullNode.setUsedKeyCount(leftSize);
    newNode.getKey(0, buffer);
    parent.internalInsertAfter(buffer, fullNode, newNode);
    delete[] buffer;
    return newNode;
}

BPlusTree::Node BPlusTree::splitInternalNode(BPlusTree::Node fullNode)
{
    char *buffer = new char[keySize];
    assert(fullNode.getUsedKeyCount() == Max_Number_Of_Branches);
    assert(!fullNode.getIsLeaf());
    BPlusTree::Node parent = fullNode.getParent();
    if (parent.isNull())
    {
        parent = allocateNode();
        parent.setIsLeaf(false);
        root = parent;
        fullNode.setParent(root);
    }

    int splitLeft = fullNode.getUsedKeyCount() / 2 - 1;
    int splitRight = splitLeft + 2;
    BPlusTree::Node newNode = allocateNode();
    int leftSize = splitLeft + 1;
    int rightSize = Max_Number_Of_Branches - 1 - leftSize;

    newNode.setIsLeaf(false);
    newNode.setUsedKeyCount(rightSize);
    newNode.setParent(parent);
    for (int i = 0; i < rightSize; i++)
    {
        fullNode.getKey(splitRight + i, buffer);
        newNode.setKey(i, buffer);
        newNode.setBranch(i + 1, fullNode.getBranch(splitRight + i + 1));
    }
    
    newNode.setBranch(0, fullNode.getBranch(splitRight));
    fullNode.setUsedKeyCount(leftSize);

    fullNode.getKey(splitLeft + 1, buffer);
    parent.internalInsertAfter(buffer, fullNode, newNode);
    delete[] buffer;

    return newNode;
}

BPlusTree::Node BPlusTree::allocateNode()
{
    uint64_t address = allocator->allocate().getOffset();
    if (address == 0x1E000)
    {
        std::cout << "Allocate " << address << std::endl;
    }
    Node node(this, address);
    node.initialize();
    return node;
}

bool BPlusTree::freeNode(Node node)
{
	return allocator->free((*storageArea)[node.address]);
}

BPlusTree::Node::Node(BPlusTree* bPlusTree, uint64_t address)
{
    this->bPlusTree = bPlusTree;
    this->address = address;
}

bool BPlusTree::Node::isValid()
{
    return bPlusTree != NULL;
}

bool BPlusTree::Node::isNull()
{
    return address == 0;
}

BPlusTree::Node BPlusTree::Node::nullNode()
{
    return Node(bPlusTree, 0);
}

BPlusTree::Node BPlusTree::Node::getParent()
{
    uint64_t parentAddress;
    bPlusTree->storageArea->getDataAt(
        address + bPlusTree->nodeParentOffset, (char*)&parentAddress, sizeof(parentAddress));
    return Node(
        this->bPlusTree,
        parentAddress
    );
}

void BPlusTree::Node::setParent(Node parent)
{
    uint64_t parentAddress = parent.address;
    bPlusTree->storageArea->setDataAt(
        address + bPlusTree->nodeParentOffset, (char*)&parentAddress, sizeof(parentAddress));
}

BPlusTree::Node BPlusTree::Node::getNext()
{
    uint64_t nextAddress;
    bPlusTree->storageArea->getDataAt(
        address + bPlusTree->nodeNextOffset, (char*)&nextAddress, sizeof(nextAddress));
    return Node(
        this->bPlusTree,
        nextAddress
    );
}

void BPlusTree::Node::setNext(BPlusTree::Node next)
{
    uint64_t nextAddress = next.address;
    bPlusTree->storageArea->setDataAt(
        address + bPlusTree->nodeNextOffset, (char*)&nextAddress, sizeof(nextAddress));
}

bool BPlusTree::Node::getIsLeaf()
{
    uint32_t isLeaf;
    bPlusTree->storageArea->getDataAt(
        address + bPlusTree->nodeIsLeafOffset, (char*)&isLeaf, sizeof(isLeaf));
    return isLeaf != 0;
}

void BPlusTree::Node::setIsLeaf(bool isLeaf)
{
    uint32_t isLeafInt = isLeaf ? 1 : 0;
    bPlusTree->storageArea->setDataAt(
        address + bPlusTree->nodeIsLeafOffset, (char*)&isLeafInt, sizeof(isLeafInt));
}

int BPlusTree::Node::getKeyCount()
{
    return bPlusTree->maxDataPerNode - 1;
}

int BPlusTree::Node::getBranchCount()
{
    return bPlusTree->maxDataPerNode;
}

uint32_t BPlusTree::Node::getUsedKeyCount()
{
    uint32_t keyCount;
    bPlusTree->storageArea->getDataAt(
        address + bPlusTree->nodeUsedCountOffset,
        (char*)&keyCount, sizeof(keyCount)
    );
    return keyCount;
}

void BPlusTree::Node::setUsedKeyCount(uint32_t count)
{
    assert(count > 0 || getIsLeaf() || address == this->bPlusTree->root.address);
    bPlusTree->storageArea->setDataAt(
        address + bPlusTree->nodeUsedCountOffset,
        (char*)&count, sizeof(count)
    );
}

BPlusTree::Node BPlusTree::Node::getBranch(int index)
{
    uint64_t branchAddress;
    bPlusTree->storageArea->getDataAt(
        address + bPlusTree->nodeBranchesOffset + index * sizeof(branchAddress),
        (char*)&branchAddress, sizeof(branchAddress)
    );
    return Node(this->bPlusTree, branchAddress);
}

bool BPlusTree::Node::setBranch(int index, Node node)
{
    assert(!node.isNull());
    uint64_t branchAddress = node.address;
    if (!bPlusTree->storageArea->setDataAt(
        address + bPlusTree->nodeBranchesOffset + index * sizeof(branchAddress),
        (char*)&branchAddress, sizeof(branchAddress)
    )) return false;
    node.setParent(*this);
    return true;
}

bool BPlusTree::Node::getKey(int index, char *buffer)
{
    return bPlusTree->storageArea->getDataAt(
        address + bPlusTree->nodeKeysOffset + index * bPlusTree->keySize,
        buffer, bPlusTree->keySize
    );
}

bool BPlusTree::Node::setKey(int index, char *buffer)
{
    return bPlusTree->storageArea->setDataAt(
        address + bPlusTree->nodeKeysOffset + index * bPlusTree->keySize,
        buffer, bPlusTree->keySize
    );
}

bool BPlusTree::Node::setBranchData(int index, char * buffer)
{
    //TODO
    return false;
}

bool BPlusTree::Node::getBranchData(int index, char * buffer)
{
    //TODO
    return false;
}

int BPlusTree::Node::compare(char * data1, char * data2)
{
    int val1 = *(int*)data1;
    int val2 = *(int*)data2;
    if (val1 > val2) return 1;
    if (val1 < val2) return -1;
    return 0;
}

void BPlusTree::Node::initialize()
{
    setParent(nullNode());
    setNext(nullNode());
    setIsLeaf(true);
    setUsedKeyCount(0);
}

int BPlusTree::Node::insertKey(char * key)
{
    char* buffer = new char[bPlusTree->keySize];
    int keyCount = getUsedKeyCount();
    int insertAt = 0;
    setUsedKeyCount(keyCount + 1);
    for (int i = keyCount; i >= 0; i--)
    {
        if (i != 0) getKey(i - 1, buffer);
        if (i != 0 && compare(buffer, key) > 0)
        {
            setKey(i, buffer);
        }
        else
        {
            setKey(i, key);
            insertAt = i;
            break;
        }
    }
    delete[] buffer;
    return insertAt;
}

int BPlusTree::Node::leafInsertAfter(char * key)
{
    return insertKey(key);
}

void BPlusTree::Node::internalInsertAfter(char * key, Node left, Node right)
{
    int insertPos = insertKey(key);
    int branchCount = getUsedKeyCount() + 1;
    assert(branchCount >= 2);
    if (branchCount == 2)
    {
        setBranch(insertPos, left);
        setBranch(insertPos + 1, right);
        left.setParent(*this);
        right.setParent(*this);
        return;
    }
    for (int i = branchCount - 1; i > insertPos; i--)
    {
        setBranch(i, getBranch(i - 1));
    }
    assert(getBranch(insertPos).address == left.address);
    setBranch(insertPos, left);
    setBranch(insertPos + 1, right);
    left.setParent(*this);
    right.setParent(*this);
}

BPlusTree::Node BPlusTree::Node::findLeaf(char* key)
{
    Node node = *this;
    char *buffer = new char[bPlusTree->keySize];
    while (!node.getIsLeaf())
    {
        int left = -1;
        int right = node.getUsedKeyCount();
        while (right - left > 1)
        {
            int middle = (left + right) / 2;
            node.getKey(middle, buffer);
            if (compare(key, buffer) < 0)
            {
                right = middle;
            }
            else
            {
                left = middle;
            }
        }
        assert(!node.getBranch(left + 1).isNull());
        node = node.getBranch(left + 1);
    }
    delete buffer;
    return node;
}

int BPlusTree::Delete(int key)
{
	char* keyData = (char*)&key;
	BPlusTree::Node leaf = root.findLeaf(keyData);
	char *keyBuffer = new char[keySize];
	int usedKey = leaf.getUsedKeyCount();
	char* temp_x = (char*)&key;
	BPlusTree::Node p = root.findLeaf(temp_x);
	for (uint32_t i = 0; i < p.getUsedKeyCount(); i++)
	{
		char key_i[4]; //TODO 4 is for int
		p.getKey(i, key_i);
		if (root.compare(key_i, temp_x) == 0)
		{
			for (int j = i; j < usedKey - 1; j++)
			{
				leaf.getKey(j + 1, keyBuffer);
				leaf.setKey(j, keyBuffer);
			}
			leaf.setUsedKeyCount(leaf.getUsedKeyCount() - 1);
			break;
		}
	}
    while (leaf.address != root.address && checkNodeHalfEmpty(leaf))
    {
        leaf = leaf.getParent();
    }
    if (leaf.address == root.address && root.getUsedKeyCount() == 0 && !root.getIsLeaf())
    {
        Node newRoot = root.getBranch(0);
        freeNode(root);
        root = newRoot;
    }
    delete[] keyBuffer;
	return 0;
}

int BPlusTree::insert(int key)//, BPlusTree::Node *root)
{
    if (root.getUsedKeyCount() == 0)
    {
        root.setUsedKeyCount(1);
        root.setKey(0, (char*)&key);
        return 0;
    }
    char* keyData = (char*)&key;
    BPlusTree::Node leaf = root.findLeaf(keyData);
    leaf.leafInsertAfter(keyData);

    auto changedNode = leaf;

    while (changedNode.getUsedKeyCount() == Max_Number_Of_Branches)
    {
        if(changedNode.getIsLeaf())
            splitLeafNode(changedNode);
        else
            splitInternalNode(changedNode);
        changedNode = changedNode.getParent();
    }
    return 0;
}

int BPlusTree::search(int key)
{
    char* temp_x = (char*)&key;
    BPlusTree::Node p = root.findLeaf(temp_x);
	for (uint32_t i = 0; i < p.getUsedKeyCount(); i++)
	{
		char key_i[4]; //TODO 4 is for int
		p.getKey(i, key_i);
		if (root.compare(key_i, temp_x) == 0)
		{
			return 1;
		}
	}
    return 0;
}

bool BPlusTree::checkNodeHalfEmpty(BPlusTree::Node node)
{
    if (node.getUsedKeyCount() > Max_Number_Of_Branches / 2)
    {
        return false;
    }
    int nodeKeyCount = node.getUsedKeyCount();
	//int nodeIndex = -1;
	Node parent = node.getParent();
    if (parent.isNull())
        return 0;
	int parentKeyCount = parent.getUsedKeyCount();
	char *keyBuffer = new char[keySize];
    Node leftNode = node.nullNode();
    Node rightNode = node.nullNode();
    int rightIndex;
    for (int i = 0; i <= parentKeyCount; i++)
    {
        if (parent.getBranch(i).address == node.address)
        {
            if (i != 0)
            {
                leftNode = parent.getBranch(i - 1);
                rightNode = parent.getBranch(i);
                rightIndex = i - 1;
            }
            else if(i != parentKeyCount)
            {
                leftNode = parent.getBranch(i);
                rightNode = parent.getBranch(i + 1);
                rightIndex = i;
            }
            else
            {
                assert(parent.address == root.address);
                return true;
            }
        }
    }
    auto leftNodeKeyCount = leftNode.getUsedKeyCount();
    auto rightNodeKeyCount = rightNode.getUsedKeyCount();
    if (node.getIsLeaf())
    {
        if (leftNodeKeyCount + rightNodeKeyCount > Max_Number_Of_Branches)
        {
            balanceLeafNode(leftNode, rightNode, rightIndex);
        }
        else
        {
            mergeLeafNode(leftNode, rightNode, rightIndex);
        }
    }
    else
    {
        if (leftNodeKeyCount + rightNodeKeyCount >= Max_Number_Of_Branches)
        {
            balanceInternalNode(leftNode, rightNode, rightIndex);
        }
        else
        {
            mergeInternalNode(leftNode, rightNode, rightIndex);
        }
    }
    return true;
}

int BPlusTree::mergeLeafNode(BPlusTree::Node left, BPlusTree::Node right, int rightIndex)
{
    // Only nodes sharing the same parent can be merged.
    assert(left.getParent().address == right.getParent().address);

    char *keyBuffer = new char[keySize];
    Node parent = left.getParent();
    int leftKeyCount = left.getUsedKeyCount();
    int rightKeyCount = right.getUsedKeyCount();

    // Check that the given index of the leaf is correct.
    assert(parent.getBranch(rightIndex + 1).address == right.address);
    assert(parent.getBranch(rightIndex).address == left.address);

    left.setUsedKeyCount(leftKeyCount + rightKeyCount);
    for (int i = 0; i < rightKeyCount; i++)
    {
        right.getKey(i, keyBuffer);
        left.setKey(leftKeyCount + i, keyBuffer);
    }
    freeNode(right);
    int parentKeyCount = parent.getUsedKeyCount();
    for (int i = rightIndex; i < parentKeyCount - 1; i++)
    {
        parent.setBranch(i + 1, parent.getBranch(i + 2));
        parent.getKey(i + 1, keyBuffer);
        parent.setKey(i, keyBuffer);
    }
    parent.setUsedKeyCount(parentKeyCount - 1);
    assert(parent.address == root.address || parentKeyCount > 1);
    return 0;
}

int BPlusTree::mergeInternalNode(BPlusTree::Node left, BPlusTree::Node right, int rightIndex)
{
    // Only nodes sharing the same parent can be merged.
    assert(left.getParent().address == right.getParent().address);

    char *keyBuffer = new char[keySize];
    Node parent = left.getParent();
    int leftKeyCount = left.getUsedKeyCount();
    int rightKeyCount = right.getUsedKeyCount();

    // Check that the given index of the leaf is correct.
    assert(parent.getBranch(rightIndex + 1).address == right.address);
    assert(parent.getBranch(rightIndex).address == left.address);

    left.setUsedKeyCount(leftKeyCount + rightKeyCount + 1);
    for (int i = 0; i < rightKeyCount; i++)
    {
        Node temp = right.getBranch(i);
        right.getKey(i, keyBuffer);
        left.setKey(leftKeyCount + 1 + i, keyBuffer);
    }
    parent.getKey(rightIndex, keyBuffer); 
    left.setKey(leftKeyCount, keyBuffer);
    for (int i = 0; i < rightKeyCount + 1; i++)
    {
        left.setBranch(leftKeyCount + 1 + i, right.getBranch(i));
    }
    for (int i = 0; i < left.getUsedKeyCount() + 1; i++)
    {
        assert(left.getBranch(i).address != 0);
    }
    freeNode(right);
    int parentKeyCount = parent.getUsedKeyCount();
    for (int i = rightIndex; i < parentKeyCount - 1; i++)
    {
        parent.setBranch(i + 1, parent.getBranch(i + 2));
        parent.getKey(i + 1, keyBuffer);
        parent.setKey(i, keyBuffer);
    }
    parent.setUsedKeyCount(parentKeyCount - 1);
    assert(parent.address == root.address || parentKeyCount > 1);
    return 0;
}

void BPlusTree::balanceLeafNode(BPlusTree::Node left, BPlusTree::Node right, int rightIndex)
{
    // Only nodes sharing the same parent can be balanced.
    assert(left.getParent().address == right.getParent().address);

    Node parent = left.getParent();
    int leftKeyCount = left.getUsedKeyCount();
    int rightKeyCount = right.getUsedKeyCount();

    // Check that the given index of the leaf is correct.
    assert(parent.getBranch(rightIndex + 1).address == right.address);
    assert(parent.getBranch(rightIndex).address == left.address);

    int totalKeyCount = leftKeyCount + rightKeyCount;
    assert(totalKeyCount >= Max_Number_Of_Branches);

    char *keyBuffer = new char[keySize * totalKeyCount];
    for (int i = 0; i < leftKeyCount; i++)
    {
        left.getKey(i, keyBuffer + i * keySize);
    }
    for (int i = 0; i < rightKeyCount; i++)
    {
        right.getKey(i, keyBuffer + (i + leftKeyCount) * keySize);
    }
    leftKeyCount = totalKeyCount / 2;
    rightKeyCount = totalKeyCount - leftKeyCount;
    left.setUsedKeyCount(leftKeyCount);
    right.setUsedKeyCount(rightKeyCount);
    for (int i = 0; i < leftKeyCount; i++)
    {
        left.setKey(i, keyBuffer + i * keySize);
    }
    for (int i = 0; i < rightKeyCount; i++)
    {
        right.setKey(i, keyBuffer + (i + leftKeyCount) * keySize);
    }
    parent.setKey(rightIndex, keyBuffer + leftKeyCount * keySize);
    delete[] keyBuffer;
}

void BPlusTree::balanceInternalNode(BPlusTree::Node left, BPlusTree::Node right, int rightIndex)
{
    // Only nodes sharing the same parent can be balanced.
    assert(left.getParent().address == right.getParent().address);

    Node parent = left.getParent();
    int leftKeyCount = left.getUsedKeyCount();
    int rightKeyCount = right.getUsedKeyCount();

    // Check that the given index of the leaf is correct.
    assert(parent.getBranch(rightIndex + 1).address == right.address);
    assert(parent.getBranch(rightIndex).address == left.address);

    int totalKeyCount = leftKeyCount + rightKeyCount + 1;
    assert(totalKeyCount >= Max_Number_Of_Branches);

    char *keyBuffer = new char[keySize * totalKeyCount];
    uint64_t *branchBuffer = new uint64_t[totalKeyCount + 1];
    for (int i = 0; i < leftKeyCount; i++)
    {
        left.getKey(i, keyBuffer + i * keySize);
    }
    for (int i = 0; i < leftKeyCount + 1; i++)
    {
        branchBuffer[i] = left.getBranch(i).address;
    }
    for (int i = 0; i < rightKeyCount + 1; i++)
    {
        branchBuffer[leftKeyCount + 1 + i] = right.getBranch(i).address;
    }
    for (int i = 0; i < rightKeyCount; i++)
    {
        right.getKey(i, keyBuffer + (1 + i + leftKeyCount) * keySize);
    }
    parent.getKey(rightIndex, keyBuffer + (leftKeyCount) * keySize);
    leftKeyCount = totalKeyCount / 2;
    rightKeyCount = totalKeyCount - leftKeyCount - 1;
    left.setUsedKeyCount(leftKeyCount);
    right.setUsedKeyCount(rightKeyCount);
    for (int i = 0; i < leftKeyCount; i++)
    {
        left.setKey(i, keyBuffer + i * keySize);
    }
    for (int i = 0; i < rightKeyCount; i++)
    {
        right.setKey(i, keyBuffer + (1 + i + leftKeyCount) * keySize);
    }
    parent.setKey(rightIndex, keyBuffer + leftKeyCount * keySize);
    for (int i = 0; i < leftKeyCount + 1; i++)
    {
        left.setBranch(i, Node(this, branchBuffer[i]));
    }
    for (int i = 0; i < rightKeyCount + 1; i++)
    {
        right.setBranch(i, Node(this, branchBuffer[leftKeyCount + 1 + i]));
    }
    delete[] keyBuffer;
}
