#include <iostream>
#include <string>
#include "../Storage/AbstractStorageArea.h"
#include "BPlusTree.h"

using namespace std;
const int Max_Number_Of_Branches = 5;

BPlusTree::BPlusTree(AbstractStorageArea* storageArea, int keySize, int valueSize)
{
    this->storageArea = storageArea;
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

BPlusTree::Node::Node(BPlusTree* bPlusTree, uint64_t address)
{
    this->bPlusTree = bPlusTree;
    this->address = address;
    for (int i = 0; i < getBranchCount(); i++)
        setBranch(i, nullNode());
    setUsedKeyCount(0);
    setParent(nullNode());
    setNext(nullNode());
    setIsLeaf(true);
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
        bPlusTree->nodeParentOffset, (char*)&parentAddress, sizeof(parentAddress));
    return Node(
        this->bPlusTree,
        parentAddress
    );
}

void BPlusTree::Node::setParent(Node parent)
{
    uint64_t parentAddress = parent.address;
    bPlusTree->storageArea->setDataAt(
        bPlusTree->nodeParentOffset, (char*)&parentAddress, sizeof(parentAddress));
}

BPlusTree::Node BPlusTree::Node::getNext()
{
    uint64_t nextAddress;
    bPlusTree->storageArea->getDataAt(
        bPlusTree->nodeNextOffset, (char*)&nextAddress, sizeof(nextAddress));
    return Node(
        this->bPlusTree,
        nextAddress
    );
}

void BPlusTree::Node::setNext(BPlusTree::Node next)
{
    uint64_t nextAddress = next.address;
    bPlusTree->storageArea->setDataAt(
        bPlusTree->nodeNextOffset, (char*)&nextAddress, sizeof(nextAddress));
}

bool BPlusTree::Node::getIsLeaf()
{
    uint32_t isLeaf;
    bPlusTree->storageArea->setDataAt(
        bPlusTree->nodeIsLeafOffset, (char*)&isLeaf, sizeof(isLeaf));
    return isLeaf != 0;
}

void BPlusTree::Node::setIsLeaf(bool isLeaf)
{
    uint32_t isLeafInt = isLeaf ? 1 : 0;
    bPlusTree->storageArea->setDataAt(
        bPlusTree->nodeIsLeafOffset, (char*)&isLeafInt, sizeof(isLeafInt));
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
    return bPlusTree->storageArea->setDataAt(
        bPlusTree->nodeUsedCountOffset,
        (char*)&keyCount, sizeof(keyCount)
    );
}

void BPlusTree::Node::setUsedKeyCount(uint32_t count)
{
    bPlusTree->storageArea->setDataAt(
        bPlusTree->nodeUsedCountOffset,
        (char*)&count, sizeof(count)
    );
}

BPlusTree::Node BPlusTree::Node::getBranch(int index)
{
    uint64_t branchAddress;
    bPlusTree->storageArea->getDataAt(
        bPlusTree->nodeBranchesOffset + index * sizeof(branchAddress),
        (char*)&branchAddress, sizeof(branchAddress)
    );
    return Node(this->bPlusTree, branchAddress);
}

bool BPlusTree::Node::setBranch(int index, Node node)
{
    uint64_t branchAddress = node.address;
    return bPlusTree->storageArea->setDataAt(
        bPlusTree->nodeBranchesOffset + index * sizeof(branchAddress),
        (char*)&branchAddress, sizeof(branchAddress)
    );
}

bool BPlusTree::Node::getKey(int index, char *buffer)
{
    return bPlusTree->storageArea->getDataAt(
        bPlusTree->nodeKeysOffset + index * bPlusTree->keySize,
        buffer, bPlusTree->keySize
    );
}

bool BPlusTree::Node::setKey(int index, char *buffer)
{
    return bPlusTree->storageArea->setDataAt(
        bPlusTree->nodeKeysOffset + index * bPlusTree->keySize,
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
    //TODO : Not implemented.
    return false;
}

BPlusTree::Node BPlusTree::Node::findLeaf(char* key)
{
    Node node = *this;
    char *buffer = new char[bPlusTree->keySize];
    while (!getIsLeaf())
    {
        int j = getKeyCount();
        for (int i = 0; i < getKeyCount(); i++)
        {
            node.getKey(i, buffer);
            if (compare(key, buffer) < 0)
            {
                j = i;
                break;
            }
        }
        node = node.getBranch(j);
    }
    delete buffer;
    return node;
}
int main()
{
	return 0;
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


void InsertIndex(BPlusTree::Node *p, int x, BPlusTree::Node *s, BPlusTree::Node *q, BPlusTree::Node *&root)
{
    while (1)
    {
        if (p == NULL)
        {
            BPlusTree::Node *r;
            //Initialize(r);
            r->setIsLeaf(false);
            r->setUsedKeyCount(1);
			char* temp_x;
			itoa(x, temp_x, 10);
            r->setKey(0,temp_x);
			r->setBranch(0, *s);
			s->setParent(*r);
			r->setBranch(1, *q);
			q->setParent(*r);
			r->bPlusTree->root = r;
            return;
        }
        int j = p->getUsedKeyCount();
		for (int i = 0; i < p->getUsedKeyCount(); i++)
		{
			char* key_i;
			p->getKey(i, key_i);
			if (atoi(key_i) > x)
			{
				j = i;
				break;
			}
		}
        if (p->getUsedKeyCount()<Max_Number_Of_Branches - 1)
        {
            for (int i = p->getUsedKeyCount(); i>j; i++)
            {
				char* key_i;
				p->getKey(i-1, key_i);
				p->setKey(i, key_i);
                p->setBranch(i+1,p->getBranch(i));
            }
			char* key_j;
			itoa(x, key_j, 10);
			p->setKey(j, key_j);
			p->setUsedKeyCount(p->getUsedKeyCount() + 1);
            p->setBranch(j+1,*q);
			q->setParent(*p);
            return;
        }
        int TempForSplit[Max_Number_Of_Branches], top = 0;
        BPlusTree::Node *pp;
        pp->setIsLeaf(false);
		for (int i = 0; i < j; i++)
		{
			char* key_i;
			p->getKey(i, key_i);
			TempForSplit[top++] = atoi(key_i);
		}
        TempForSplit[top++] = x;
        for (int i = j; i<Max_Number_Of_Branches - 1; i++)
		{
			char* key_i;
			p->getKey(i, key_i);
			TempForSplit[top++] = atoi(key_i);
		}
        p->setUsedKeyCount(0);
		for (int i = 0; i < (top - 1) >> 1; i++)
		{
			char* temp_x;
			itoa(TempForSplit[i], temp_x, 10);
			p->setKey(p->getUsedKeyCount(), temp_x);
			p->setUsedKeyCount(p->getUsedKeyCount() + 1);
		}
        for (int i = (top + 1) >> 1; i<top; i++)
        {
			char* temp_x;
			itoa(TempForSplit[i], temp_x, 10);
			pp->setKey(q->getUsedKeyCount() + 1, temp_x);
			q->setUsedKeyCount(q->getUsedKeyCount() + 1);
			pp->setBranch(q->getUsedKeyCount(), p->getBranch(i));
			p->getBranch(q->getUsedKeyCount()).setParent(*pp);
            //p->branch[i] = NULL;TODO:
        }
        pp->setBranch(0,*q);
		q->setParent(*pp);
        s = p;
        p->setParent(*p);
        q = pp;
        x = TempForSplit[(top - 1) >> 1];
    }
    return;
}

int Insert(int x, BPlusTree::Node *root)
{
    if (root == NULL)
    {
        //Initialize(root);
        root->setUsedKeyCount(1);
		char* temp_x;
		itoa(x,temp_x, 10);
        root->setKey(0,temp_x);
        return 0;
    }
	char* temp_x;
	itoa(x, temp_x, 10);
    BPlusTree::Node _p = root->findLeaf(temp_x);
	BPlusTree::Node *p = &_p;
    int j = p->getUsedKeyCount();
    for (int i = 0; i<p->getUsedKeyCount(); i++)
    {
		char* key_i;
		p->getKey(i, key_i);
		if (atoi(key_i) == x)
            return 1;
        if (atoi(key_i) > x)
        {
            j = i;
            break;
        }
    }
    if (p->getUsedKeyCount()<Max_Number_Of_Branches - 1)
    {
		for (int i = p->getUsedKeyCount(); i > j; i--)
		{
			char* key_i;
			p->getKey(i - 1, key_i);
			p->setKey(i, key_i);
		}
		p->setKey(j, temp_x);
        p->setUsedKeyCount(p->getUsedKeyCount()+1);
        Refresh(x, *p);
        return 0;
    }
    int TempForSplit[Max_Number_Of_Branches], top = 0;
    BPlusTree::Node *q;
    //Initialize(q);
	for (int i = 0; i < j; i++)
	{
		char* key_i;
		p->getKey(i, key_i);
		TempForSplit[top++] = atoi(key_i);
	}
    TempForSplit[top++] = x;
    for (int i = j; i<Max_Number_Of_Branches - 1; i++)
	{
		char* key_i;
		p->getKey(i, key_i);
		TempForSplit[top++] = atoi(key_i);
	}
    q->setNext(p->getNext());
    p->setNext(*q);
    p->setUsedKeyCount(0);
	for (int i = 0; i <= (top - 1) >> 1; i++)
	{
		p->setKey(p->getUsedKeyCount(), temp_x);
		p->setUsedKeyCount(p->getUsedKeyCount() + 1);
	}
    for (int i = (top + 1) >> 1; i<top; i++)
	{
		q->setKey(q->getUsedKeyCount() + 1, temp_x);
		q->setUsedKeyCount(q->getUsedKeyCount() + 1);
	}
	char* key0;
	q->getKey(0, key0);
    InsertIndex(&(p->getParent()), atoi(key0), p, q, root);
    return 0;
}

int Search(int x, BPlusTree::Node *root)
{
	char* temp_x;
	itoa(x, temp_x, 10);
    BPlusTree::Node *p = &root->findLeaf(temp_x);
	for (uint32_t i = 0; i < p->getUsedKeyCount(); i++)
	{
		char* key_i;
		p->getKey(i, key_i);
		if (atoi(key_i) == x)
		{
			return 1;
		}
	}
    return 0;
}

void DeleteIndex(BPlusTree::Node *p, int k, BPlusTree::Node *&root)
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
