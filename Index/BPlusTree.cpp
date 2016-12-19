#include <cassert>
#include <iostream>
#include <string>
#include "Storage/AbstractStorageArea.h"
#include "BPlusTree.h"
#include "Storage/BlockAllocator.h"
using namespace std;
const int Max_Number_Of_Branches = 5;

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
    int splitLeft = (fullNode.getUsedKeyCount() - 1) / 2;
    int splitRight = splitLeft + 1;
    BPlusTree::Node newNode = allocateNode();
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
    return newNode;
}

BPlusTree::Node BPlusTree::splitInternalNode(BPlusTree::Node fullNode)
{
    assert(fullNode.getUsedKeyCount() == Max_Number_Of_Branches);
    char *buffer = new char[keySize];
    assert(fullNode.getUsedKeyCount() == Max_Number_Of_Branches);
    int splitLeft = (fullNode.getUsedKeyCount() - 1) / 2;
    int splitRight = splitLeft + 1;
    BPlusTree::Node newNode = allocateNode();
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
    return bPlusTree->storageArea->setDataAt(
        address + bPlusTree->nodeBranchesOffset + index * sizeof(branchAddress),
        (char*)&branchAddress, sizeof(branchAddress)
    );
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

BPlusTree::Node BPlusTree::Node::findLeaf(char* key)
{
    Node node = *this;
    char *buffer = new char[bPlusTree->keySize];
    while (!node.getIsLeaf())
    {
        //std::cout << "# " << node.address << std::endl;
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
        node = node.getBranch(j);
    }
    delete buffer;
    return node;
}
/*int main()
{
	return 0;
}*/
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


void BPlusTree::insertIndex(BPlusTree::Node parent, int x, 
    BPlusTree::Node left, BPlusTree::Node right)
{
    assert(!left.isNull());
    assert(!right.isNull());
    char* keyData = (char*)&x;
    while (true)
    {
        if (parent.isNull())
        {
            BPlusTree::Node newRoot = allocateNode();
            newRoot.setIsLeaf(false);
            newRoot.setUsedKeyCount(1);
            newRoot.setKey(0, keyData);
			newRoot.setBranch(0, left);
			left.setParent(newRoot);
			newRoot.setBranch(1, right);
			right.setParent(newRoot);
			newRoot.bPlusTree->root = newRoot;
            return;
        }
        char *keyBuffer = new char[keySize];
        int j = parent.getUsedKeyCount();
		for (int i = 0; i < parent.getUsedKeyCount(); i++)
		{
			parent.getKey(i, keyBuffer);
			if (parent.compare(keyBuffer, keyData) > 0)
			{
				j = i;
				break;
			}
		}
        if (parent.getUsedKeyCount()<Max_Number_Of_Branches - 1)
        {
            for (int i = parent.getUsedKeyCount(); i>j; i--)
            {
				parent.getKey(i-1, keyBuffer);
				parent.setKey(i, keyBuffer);
                parent.setBranch(i+1,parent.getBranch(i));
            }
			parent.setKey(j, keyData);
			parent.setUsedKeyCount(parent.getUsedKeyCount() + 1);
            parent.setBranch(j+1, right);
			right.setParent(parent);
            return;
        }
        int TempForSplit[Max_Number_Of_Branches], top = 0;
        BPlusTree::Node newNode = allocateNode();
        newNode.setIsLeaf(false);
		for (int i = 0; i < j; i++)
		{
			parent.getKey(i, keyBuffer);
			TempForSplit[top++] = *(int*)keyBuffer;
		}
        TempForSplit[top++] = x;
        for (int i = j; i<Max_Number_Of_Branches - 1; i++)
		{
			parent.getKey(i, keyBuffer);
			TempForSplit[top++] = *(int*)keyBuffer;
		}
        parent.setUsedKeyCount(0);
		for (int i = 0; i < (top - 1) >> 1; i++)
		{
			parent.setKey(parent.getUsedKeyCount(), (char*)&TempForSplit[i]);
			parent.setUsedKeyCount(parent.getUsedKeyCount() + 1);
		}
        std::cout << "Parent Key:" << parent.getUsedKeyCount();

        for (int i = (top + 1) >> 1; i<top; i++)
        {
            newNode.setKey(newNode.getUsedKeyCount() + 1, (char*)&TempForSplit[i]);
            newNode.setBranch(newNode.getUsedKeyCount(), parent.getBranch(i));
            newNode.setUsedKeyCount(newNode.getUsedKeyCount() + 1);
			//newNode.setKey(right.getUsedKeyCount() + 1, (char*)&TempForSplit[i]);
			//right.setUsedKeyCount(right.getUsedKeyCount() + 1);
			//newNode.setBranch(right.getUsedKeyCount(), parent.getBranch(i));
			//parent.getBranch(right.getUsedKeyCount()).setParent(newNode);
            //parent.setBranch(i, parent.nullNode());
        }

        std::cout << "New Node Key:" << newNode.getUsedKeyCount();

        delete[] keyBuffer;
        newNode.setBranch(0, right);
		right.setParent(newNode);
        left = parent;
        parent = parent.getParent();
        right = newNode;
        x = TempForSplit[(top - 1) >> 1];
    }
    return;
}

int BPlusTree::insert(int key, BPlusTree::Node *root)
{
    if (root->getUsedKeyCount() == 0)
    {
        root->setUsedKeyCount(1);
        root->setKey(0, (char*)&key);
        return 0;
    }
    char* keyData = (char*)&key;
    BPlusTree::Node _p = root->findLeaf(keyData);
    BPlusTree::Node *leaf = &_p;
    int j = leaf->getUsedKeyCount();
    for (int i = 0; i<leaf->getUsedKeyCount(); i++)
    {
        auto key_i = new char[root->bPlusTree->keySize];
        leaf->getKey(i, key_i);
        if (root->compare(key_i, keyData) == 0)
            return 1;
        if (root->compare(key_i, keyData) > 0)
        {
            j = i;
            break;
        }
        delete[] key_i;
    }
    //if (leaf->getUsedKeyCount()<Max_Number_Of_Branches - 1)
    //{
    for (int i = leaf->getUsedKeyCount(); i > j; i--)
    {
        char* key_i = new char[root->bPlusTree->keySize];
        leaf->getKey(i - 1, key_i);
        leaf->setKey(i, key_i);
    }
    leaf->setKey(j, keyData);
    leaf->setUsedKeyCount(leaf->getUsedKeyCount() + 1);
    //Refresh(key, *leaf);
    if (leaf->getUsedKeyCount() == Max_Number_Of_Branches)
    {
        Node q = splitLeafNode(*leaf);
        char* key0 = new char[keySize];
        q.getKey(0, key0);
        insertIndex(leaf->getParent(), *(int*)key0, *leaf, q);
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
}
*/
