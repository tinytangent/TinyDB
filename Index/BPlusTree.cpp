#include <cassert>
#include <iostream>
#include <string>
#include "../Storage/AbstractStorageArea.h"
#include "BPlusTree.h"
#include "../Storage/BlockAllocator.h"
using namespace std;
const int Max_Number_Of_Branches = 50;

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
    setUsedKeyCount(0);
    setIsLeaf(true);
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
		std::cout << 7;
        int j = node.getUsedKeyCount();
        for (int i = 0; i < node.getUsedKeyCount(); i++)
        {
            node.getKey(i, buffer);
            if (compare(key, buffer) < 0)
            {
                j = i;
                break;
            }
        }
        assert(!node.getBranch(j).isNull());
		if (j > node.getUsedKeyCount())
		{
			std::cout << "error" << std::endl;
			break;
		}
        node = node.getBranch(j);
    }
    delete buffer;
    return node;
}

void Refresh(int x, BPlusTree::Node p)
{
    BPlusTree::Node q = p.getParent(), r = p;
    while (!q.isNull())
    {
        int j;
		char* key0;
		r.getKey(0, key0);

        if (x == atoi(key0) && q.getBranch(0).address != r.address)
        {
            for (int i = 0; i<q.getUsedKeyCount(); i++)
                if (q.getBranch(i+1).address == r.address)
                {
                    j = i;
                    break;
                }
        }
        else
            break;
		char* temp_x;
		itoa(x, temp_x, 10);
		q.setBranchData(j, temp_x);
        r = q;
        q = q.getParent();
    }
    return;
}
int BPlusTree::Delete(int key)
{
	char* keyData = (char*)&key;
	BPlusTree::Node leaf = root.findLeaf(keyData);
	std::cout << 0 << std::endl;
	char *keyBuffer = new char[keySize];
	char *dataBuffer = new char[keySize];
	int usedKey = leaf.getUsedKeyCount();
	for (int i = 0; i < usedKey; i++)
	{
		leaf.getKey(i, keyBuffer);
		if (atoi(keyBuffer) == key)
		{
			for (int j = i; j < usedKey - 1; j++)
			{
				leaf.getKey(j + 1, keyBuffer);
				leaf.setKey(j, keyBuffer);
				leaf.getBranchData(j + 1, dataBuffer);//TODO:
				leaf.setBranchData(j, dataBuffer);
			}
			leaf.setUsedKeyCount(leaf.getUsedKeyCount() - 1);
		}
	}
	TryMergeLeafNode(leaf);
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

int Search(int x, BPlusTree::Node *root)
{
    char* temp_x = (char*)&x;
    BPlusTree::Node *p = &root->findLeaf(temp_x);
	for (uint32_t i = 0; i < p->getUsedKeyCount(); i++)
	{
		char key_i[4]; //TODO 4 is for int
		p->getKey(i, key_i);
		if (root->compare(key_i, temp_x) == 0)
		{
			return 1;
		}
	}
    return 0;
}

int BPlusTree::TryMergeLeafNode(BPlusTree::Node node)
{
	std::cout << 1 << std::endl;
	int nodeIndex = -1;
	Node parent = node.getParent();
	int nodeKeyCount = node.getUsedKeyCount();
	int parentKeyCount = parent.getUsedKeyCount();
	for (int i = 0; i < parentKeyCount; i++)
	{
		if (parent.getBranch(i).address == node.address)
		{
			nodeIndex = i;
			break;
		}
	}
	assert(node.address == root.address || nodeIndex != -1);
	char *keyBuffer = new char[keySize];
	if (nodeIndex >= 0)
	{
		if (nodeKeyCount > node.getKeyCount()/2)
		{
			return 0;
		}
		if (nodeIndex > 0)
		{
			if (nodeKeyCount + parent.getBranch(nodeIndex - 1).getUsedKeyCount() > node.getKeyCount())
			{
				while (nodeKeyCount < parent.getBranch(nodeIndex - 1).getUsedKeyCount())
				{
					node.setUsedKeyCount(node.getUsedKeyCount() + 1);
					for (int i = nodeKeyCount; i > 0; i--)
					{
						node.getKey(i, keyBuffer);
						node.setKey(i - 1, keyBuffer);
						//TODO:branch data
					}
					nodeKeyCount++;
					parent.getBranch(nodeIndex - 1).getKey(parent.getBranch(nodeIndex - 1).getUsedKeyCount() - 1, keyBuffer);
					node.setKey(0, keyBuffer);
					//TODO:branch data
					parent.getBranch(nodeIndex - 1).setUsedKeyCount(parent.getBranch(nodeIndex - 1).getUsedKeyCount() - 1);
				}
				return 0;
			}
			else
			{
				return MergeLeafNode(parent.getBranch(nodeIndex - 1), node, nodeIndex);
			}
		}
		else
		{
			if (nodeKeyCount + parent.getBranch(1).getUsedKeyCount() > node.getKeyCount())
			{
				while (nodeKeyCount < parent.getBranch(1).getUsedKeyCount())
				{
					node.setUsedKeyCount(node.getUsedKeyCount() + 1); 
					parent.getBranch(1).getKey(0, keyBuffer);
					node.setKey(nodeKeyCount, keyBuffer);
					//TODO:branch data
					for (int i = 0; i < parent.getBranch(1).getUsedKeyCount() - 1; i++)
					{
						parent.getBranch(1).getKey(i + 1, keyBuffer);
						parent.getBranch(1).setKey(i, keyBuffer);
						//TODO:branch data
					}
					nodeKeyCount++;
					parent.getBranch(1).setUsedKeyCount(parent.getBranch(1).getUsedKeyCount() - 1);
				}
				return 0;
			}
			else
			{
				return MergeLeafNode(node, parent.getBranch(1), 1);
			}
		}
	}
	else
	{
		return -1;
	}
}

int BPlusTree::TryMergeInternalNode(BPlusTree::Node node)
{
	int nodeIndex = -1;
	Node parent = node.getParent();
	int nodeKeyCount = node.getUsedKeyCount();
	int parentKeyCount = parent.getUsedKeyCount();
	for (int i = 0; i < parentKeyCount; i++)
	{
		if (parent.getBranch(i).address == node.address)
		{
			nodeIndex = i;
			break;
		}
	}
	assert(node.address == root.address || nodeIndex != -1);
	char *keyBuffer = new char[keySize];
	if (nodeIndex >= 0)
	{
		if (nodeKeyCount > node.getKeyCount() / 2)
		{
			return 0;
		}
		if (nodeIndex > 0)
		{
			if (nodeKeyCount + parent.getBranch(nodeIndex - 1).getUsedKeyCount() >= node.getKeyCount())
			{
				while (nodeKeyCount < parent.getBranch(nodeIndex - 1).getUsedKeyCount())
				{
					node.setUsedKeyCount(node.getUsedKeyCount() + 1);
					for (int i = nodeKeyCount; i > 0; i--)
					{
						node.getKey(i, keyBuffer);
						node.setKey(i - 1, keyBuffer);
						node.setBranch(i - 1, node.getBranch(i));
					}
					nodeKeyCount++;
					parent.getBranch(nodeIndex - 1).getKey(parent.getBranch(nodeIndex - 1).getUsedKeyCount() - 1, keyBuffer);
					node.setKey(0, keyBuffer);
					node.setBranch(0, parent.getBranch(nodeIndex - 1).getBranch(parent.getBranch(nodeIndex - 1).getUsedKeyCount() - 1));
					parent.getBranch(nodeIndex - 1).setUsedKeyCount(parent.getBranch(nodeIndex - 1).getUsedKeyCount() - 1);
				}
				return 0;
			}
			else
			{
				return MergeInternalNode(parent.getBranch(nodeIndex - 1), node, nodeIndex);
			}
		}
		else
		{
			if (nodeKeyCount + parent.getBranch(1).getUsedKeyCount() >= node.getKeyCount())
			{
				while (nodeKeyCount < parent.getBranch(1).getUsedKeyCount())
				{
					node.setUsedKeyCount(node.getUsedKeyCount() + 1);
					parent.getBranch(1).getKey(0, keyBuffer);
					node.setKey(nodeKeyCount, keyBuffer);
					node.setBranch(nodeKeyCount, parent.getBranch(1).getBranch(0));
					for (int i = 0; i < parent.getBranch(1).getUsedKeyCount() - 1; i++)
					{
						parent.getBranch(1).getKey(i + 1, keyBuffer);
						parent.getBranch(1).setKey(i, keyBuffer);
						parent.getBranch(1).setBranch(i, parent.getBranch(1).getBranch(i + 1));
					}
					nodeKeyCount++;
					parent.getBranch(1).setUsedKeyCount(parent.getBranch(1).getUsedKeyCount() - 1);
				}
				return 0;
			}
			else
			{
				return MergeInternalNode(node, parent.getBranch(1), 1);
			}
		}
	}
	else
	{
		return -1;
	}
}
int BPlusTree::MergeInternalNode(BPlusTree::Node left, BPlusTree::Node right, int rightIndex)
{
	assert(left.getParent().address == right.getParent().address);
	char *keyBuffer = new char[keySize];
	Node parent = left.getParent();
	int leftKeyCount = left.getUsedKeyCount();
	int rightKeyCount = right.getUsedKeyCount();
	left.setUsedKeyCount(leftKeyCount + rightKeyCount + 1);
	parent.getKey(rightIndex, keyBuffer);
	left.setKey(leftKeyCount, keyBuffer);
	for (int i = 0; i < rightKeyCount; i++)
	{
		Node temp = right.getBranch(i);
		left.setBranch(leftKeyCount + i, temp);
		right.getKey(i, keyBuffer);
		left.setKey(leftKeyCount + i + 1, keyBuffer);
	}
	freeNode(right);
	int parentKeyCount = parent.getUsedKeyCount();
	for (int i = rightIndex; i < parentKeyCount - 1; i++)
	{
		parent.setBranch(i, parent.getBranch(i + 1));
		parent.getKey(i + 1, keyBuffer);
		parent.setKey(i, keyBuffer);
	}
	parent.setUsedKeyCount(parentKeyCount - 1);
	int parentIndex = -1;
	for (int i = 0; i < parent.getParent().getUsedKeyCount(); i++)
	{
		if (parent.getParent().getBranch(i).address == parent.address)
		{
			parentIndex = i;
			break;
		}
	}
	if (parentIndex >= 0)
	{
		return TryMergeInternalNode(parent);
	}
	else
	{
		if (parent.address == root.address)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	return 0;
}
int BPlusTree::MergeLeafNode(BPlusTree::Node left, BPlusTree::Node right, int rightIndex)
{
	assert(left.getParent().address == right.getParent().address);
	char *keyBuffer = new char[keySize];
	Node parent = left.getParent();
	int leftKeyCount = left.getUsedKeyCount();
	int rightKeyCount = right.getUsedKeyCount();
	left.setUsedKeyCount(leftKeyCount + rightKeyCount);
	for (int i = 0; i < rightKeyCount; i++)
	{
		/*Node temp = right.getBranch(i);
		left.setBranch(leftKeyCount + i, temp);*/
		//TODO: Set branch data
		right.getKey(i, keyBuffer);
		left.setKey(leftKeyCount + i, keyBuffer);
	}
	freeNode(right);
	int parentKeyCount = parent.getUsedKeyCount();
	for (int i = rightIndex; i < parentKeyCount - 1; i++)
	{
		parent.setBranch(i, parent.getBranch(i + 1));
		parent.getKey(i + 1, keyBuffer);
		parent.setKey(i, keyBuffer);
	}
	parent.setUsedKeyCount(parentKeyCount - 1);
	int parentIndex = -1;
	for (int i = 0; i < parent.getParent().getUsedKeyCount(); i++)
	{
		if (parent.getParent().getBranch(i).address == parent.address)
		{
			parentIndex = i;
			break;
		}
	}
	if (parentIndex >= 0)
	{
		return TryMergeInternalNode(parent);
	}
	else
	{
		if (parent.address == root.address)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
}
	/*void DeleteIndex(BPlusTree::Node *p, int k, BPlusTree::Node *&root)
{
    for (int i = k + 1; i<p->getUsedKeyCount(); i--)
    {
		char* key_i;
		p->getKey(i, key_i);
		p->setKey(i-1, key_i);
		p->setBranch(i, p->getBranch(i + 1));
    }
    p->setUsedKeyCount(p->getUsedKeyCount() + 1);
    while (1)
    {
        if (p->getUsedKeyCount() == 0)
        {
            BPlusTree::Node *r = &p->getBranch(0);
            //r->father = NULL;TODO:
            delete p;
            root = r;
            return;
        }
        if (p == root || p->getUsedKeyCount() >= (Max_Number_Of_Branches - 1) >> 1)
            return;
        int j;
        BPlusTree::Node *q = &p->getParent();
		for (int i = 0; i <= q->getUsedKeyCount(); i++)
		{
			if (q->getBranch(i).address == p->address)
			{
				j = i;
				break;
			}
		}
        if (j>0)
        {
            BPlusTree::Node *r = &q->getBranch(j-1);
            if (r->getUsedKeyCount()>(Max_Number_Of_Branches - 1) >> 1)
            {
                for (int i = p->getUsedKeyCount(); i>0; i--)
                {
					char* key_i;
					p->getKey(i - 1, key_i);
					p->setKey(i, key_i);
					p->setBranch(i + 1, p->getBranch(i));
                }
				p->setBranch(1, p->getBranch(0));
                p->setUsedKeyCount(p->getUsedKeyCount() + 1);
				char* key_j;
				q->getKey(j - 1, key_j);
				p->setKey(0, key_j);
				p->setBranch(0, r->getBranch(r->getUsedKeyCount()));
				r->setUsedKeyCount(r->getUsedKeyCount() - 1);
				r->getKey(r->getUsedKeyCount(), key_j);
				q->setKey(j - 1, key_j);
            }
            return;
        }
        if (j<q->getUsedKeyCount())
        {
			BPlusTree::Node *r = &q->getBranch(j + 1);
            if (r->getUsedKeyCount()>(Max_Number_Of_Branches - 1) >> 1)
            {
				char* key_j;
				q->getKey(j,key_j);
				p->setKey(p->getUsedKeyCount(), key_j);
				p->setUsedKeyCount(p->getUsedKeyCount() + 1);
                p->setBranch(p->getUsedKeyCount(),r->getBranch(0));
				r->getKey(0, key_j);
				q->setKey(j, key_j);
				r->setUsedKeyCount(r->getUsedKeyCount() - 1);
                for (int i = 0; i<r->getUsedKeyCount(); i++)
                {
					r->getKey(i + 1, key_j);
					r->setKey(i, key_j);
                    //r->key[i] = r->key[i + 1];
					r->setBranch(i, r->getBranch(i + 1));
                }
				r->setBranch(r->getUsedKeyCount(), r->getBranch(r->getUsedKeyCount() + 1));
            }
            return;
        }
        if (j<q->getUsedKeyCount())
        {
			BPlusTree::Node *r = &q->getBranch(j + 1);
			char* key_j;
			q->getKey(j, key_j);
			p->setKey(p->getUsedKeyCount(), key_j);
			p->setUsedKeyCount(p->getUsedKeyCount() + 1);
            p->setBranch(p->getUsedKeyCount(),r->getBranch(0));
            for (int i = 0; i<r->getUsedKeyCount(); i++)
            {
				r->getKey(i, key_j);
				p->setKey(p->getUsedKeyCount(), key_j);
				p->setUsedKeyCount(p->getUsedKeyCount() + 1);
				p->setBranch(p->getUsedKeyCount(), r->getBranch(i+1));
            }
            delete r;
            for (int i = j + 1; i<q->getUsedKeyCount(); i++)
            {
				q->getKey(i, key_j);
				q->setKey(i - 1, key_j);
				q->setBranch(i, q->getBranch(i + 1));
            }
			q->setUsedKeyCount(q->getUsedKeyCount() - 1);
            p = q;
            continue;
        }
		BPlusTree::Node *r = &q->getBranch(j - 1);
		char* key_j;
		q->getKey(j - 1, key_j);
		r->setKey(r->getUsedKeyCount(), key_j);
		r->setUsedKeyCount(r->getUsedKeyCount() + 1);
        //r->key[r->keyTally++] = q->key[j - 1];
        //r->branch[r->keyTally] = p->getBranch(0);
		r->setBranch(r->getUsedKeyCount(), p->getBranch(0));
        for (int i = 0; i<p->getUsedKeyCount(); i++)
        {
			p->getKey(i, key_j);
			r->setKey(r->getUsedKeyCount(), key_j);
			r->setUsedKeyCount(r->getUsedKeyCount() + 1);
			r->setBranch(r->getUsedKeyCount(), p->getBranch(i + 1));
            //r->key[r->keyTally++] = p->key[i];
            //r->branch[r->keyTally] = p->branch[i + 1];
        }
        delete p;
        for (int i = j; i<q->getUsedKeyCount(); i++)
        {
			q->getKey(i, key_j);
			q->setKey(i-1, key_j);
			q->setBranch(i, q->getBranch(i + 1));
        }
		q->setUsedKeyCount(q->getUsedKeyCount() - 1);
        p = q;
    }
    return;
}

void Delete(int x, BPlusTree::Node *&root)
{
	char* temp_x;
	itoa(x, temp_x, 10);
    BPlusTree::Node *p = &root->findLeaf(temp_x);
    int j;
	for (int i = 0; i < p->getUsedKeyCount(); i++)
	{
		char* key_i;
		p->getKey(i, key_i);
		if (key_i == temp_x)
		{
			j = i;
			break;
		}
	}

	for (int i = j; i < p->getUsedKeyCount() - 1; i++)
	{
		char* key_i;
		p->getKey(i + 1, key_i);
		p->setKey(i, key_i);
	}
    p->setUsedKeyCount(p->getUsedKeyCount() + 1);
    if (p == root || p->getUsedKeyCount() >= (Max_Number_Of_Branches - 1) >> 1)
        return;
    BPlusTree::Node *q = &p->getParent();
    for (int i = 0; i <= q->getUsedKeyCount(); i++)
        if (q->getBranch(i).address == p->address)
        {
            j = i;
            break;
        }
    if (j>0)
    {
        BPlusTree::Node *r = &q->getBranch(j-1);
        if (r->getUsedKeyCount()>(Max_Number_Of_Branches - 1) >> 1)
        {
            for (int i = p->getUsedKeyCount(); i>0; i--)
			{
				char* key_i;
				p->getKey(i - 1, key_i);
				p->setKey(i, key_i);
			}
			p->setUsedKeyCount(p->getUsedKeyCount() + 1);
            //p->key[0] = r->key[r->keyTally - 1];
			char* key_i;
			r->getKey(r->getUsedKeyCount() - 1, key_i);
			p->setKey(0, key_i);
			r->setUsedKeyCount(r->getUsedKeyCount() - 1);
        }
		char* key_i;
		p->getKey(0, key_i);
        Refresh(atoi(key_i), *p);
        return;
    }
    if (j<q->getUsedKeyCount())
    {
        BPlusTree::Node *r = &q->getBranch(j + 1);
        if (r->getUsedKeyCount()>(Max_Number_Of_Branches - 1) >> 1)
        {
			char* temp;
			r->getKey(0, temp);
			p->setKey(p->getUsedKeyCount(), temp);
			p->setUsedKeyCount(p->getUsedKeyCount() + 1);
			r->setUsedKeyCount(r->getUsedKeyCount() - 1);
			for (int i = 0; i < r->getUsedKeyCount(); i++)
			{
				char* key_i;
				r->getKey(i + 1, key_i);
				r->setKey(i, key_i);
			}
        }
		char* key_i;
		r->getKey(0, key_i);
        Refresh(atoi(key_i), *r);
        return;
    }
    if (j<q->getUsedKeyCount())
    {
		BPlusTree::Node *r = &q->getBranch(j + 1);
		for (int i = 0; i < r->getUsedKeyCount(); i++)
		{
			char* temp;
			r->getKey(i, temp);
			p->setKey(p->getUsedKeyCount(), temp);
			p->setUsedKeyCount(p->getUsedKeyCount() + 1);
		}
		p->setNext(r->getNext());
        delete r;
        DeleteIndex(q, j, root);
        return;
    }
	BPlusTree::Node *r = &q->getBranch(j - 1);
	for (int i = 0; i < p->getUsedKeyCount(); i++)
	{
		char* temp;
		p->getKey(i, temp);
		r->setKey(r->getUsedKeyCount(), temp);
		r->setUsedKeyCount(r->getUsedKeyCount() + 1);
	}
	r->setNext(p->getNext());
    delete p;
    DeleteIndex(q, j - 1, root);
    return;
}*/
