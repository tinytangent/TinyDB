#include <iostream>
#include <string>
#include "Storage/AbstractStorageArea.h"
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
            node.GetKey(i, buffer);
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

void Refresh(int x, BPlusTree::Node p)
{
    BPlusTree::Node q = p.getParent(), r = p;
    while (!q.isNull())
    {
        int j;
        if (x == r.key[0] && q.branch[0] != r)
        {
            for (int i = 0; i<q->keyTally; i++)
                if (q->branch[i + 1] == r)
                {
                    j = i;
                    break;
                }
        }
        else
            break;
        q->key[j] = x;
        r = q;
        q = q->father;
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
            Initialize(r);
            r->leaf = false;
            r->keyTally = 1;
            r->key[0] = x;
            r->branch[0] = s;
            s->father = r;
            r->branch[1] = q;
            q->father = r;
            root = r;
            return;
        }
        int j = p->keyTally;
        for (int i = 0; i<p->keyTally; i++)
            if (p->key[i]>x)
            {
                j = i;
                break;
            }
        if (p->keyTally<Max_Number_Of_Branches - 1)
        {
            for (int i = p->keyTally; i>j; i++)
            {
                p->key[i] = p->key[i - 1];
                p->branch[i + 1] = p->branch[i];
            }
            p->key[j] = x;
            p->keyTally++;
            p->branch[j + 1] = q;
            q->father = p;
            return;
        }
        int TempForSplit[Max_Number_Of_Branches], top = 0;
        BPlusTree::Node *pp;
        Initialize(pp);
        pp->leaf = false;
        for (int i = 0; i<j; i++)
            TempForSplit[top++] = p->key[i];
        TempForSplit[top++] = x;
        for (int i = j; i<Max_Number_Of_Branches - 1; i++)
            TempForSplit[top++] = p->key[i];
        p->keyTally = 0;
        for (int i = 0; i<(top - 1) >> 1; i++)
            p->key[p->keyTally++] = TempForSplit[i];
        for (int i = (top + 1) >> 1; i<top; i++)
        {
            pp->key[q->keyTally++] = TempForSplit[i];
            pp->branch[q->keyTally] = p->branch[i];
            pp->branch[q->keyTally]->father = pp;
            p->branch[i] = NULL;
        }
        pp->branch[0] = q;
        q->father = pp;
        s = p;
        p = p->father;
        q = pp;
        x = TempForSplit[(top - 1) >> 1];
    }
    return;
}

int Insert(int x, BPlusTree::Node *&root)
{
    if (root == NULL)
    {
        Initialize(root);
        root->keyTally = 1;
        root->key[0] = x;
        return 0;
    }
    BPlusTree::Node *p = FindLeaf(x, root);
    int j = p->keyTally;
    for (int i = 0; i<p->keyTally; i++)
    {
        if (p->key[i] == x)
            return 1;
        if (p->key[i]>x)
        {
            j = i;
            break;
        }
    }
    if (p->keyTally<Max_Number_Of_Branches - 1)
    {
        for (int i = p->keyTally; i>j; i--)
            p->key[i] = p->key[i - 1];
        p->key[j] = x;
        p->keyTally++;
        Refresh(x, p);
        return 0;
    }
    int TempForSplit[Max_Number_Of_Branches], top = 0;
    BPlusTree::Node *q;
    Initialize(q);
    for (int i = 0; i<j; i++)
        TempForSplit[top++] = p->key[i];
    TempForSplit[top++] = x;
    for (int i = j; i<Max_Number_Of_Branches - 1; i++)
        TempForSplit[top++] = p->key[i];
    q->Sequential_Next = p->Sequential_Next;
    p->Sequential_Next = q;
    p->keyTally = 0;
    for (int i = 0; i <= (top - 1) >> 1; i++)
        p->key[p->keyTally++] = TempForSplit[i];
    for (int i = (top + 1) >> 1; i<top; i++)
        q->key[q->keyTally++] = TempForSplit[i];
    InsertIndex(p->father, q->key[0], p, q, root);
    return 0;
}

int Search(int x, BPlusTree::Node *root)
{
    BPlusTree::Node *p = FindLeaf(x, root);
    for (int i = 0; i<p->keyTally; i++)
        if (p->key[i] == x)
            return 1;
    return 0;
}

void DeleteIndex(BPlusTree::Node *p, int k, BPlusTree::Node *&root)
{
    for (int i = k + 1; i<p->keyTally; i--)
    {
        p->key[i - 1] = p->key[i];
        p->branch[i] = p->branch[i + 1];
    }
    p->keyTally--;
    while (1)
    {
        if (p->keyTally == 0)
        {
            BPlusTree::Node *r = p->branch[0];
            r->father = NULL;
            delete p;
            root = r;
            return;
        }
        if (p == root || p->keyTally >= (Max_Number_Of_Branches - 1) >> 1)
            return;
        int j;
        BPlusTree::Node *q = p->father;
        for (int i = 0; i <= q->keyTally; i++)
            if (q->branch[i] == p)
            {
                j = i;
                break;
            }
        if (j>0)
        {
            BPlusTree::Node *r = q->branch[j - 1];
            if (r->keyTally>(Max_Number_Of_Branches - 1) >> 1)
            {
                for (int i = p->keyTally; i>0; i--)
                {
                    p->key[i] = p->key[i - 1];
                    p->branch[i + 1] = p->branch[i];
                }
                p->branch[1] = p->branch[0];
                p->keyTally++;
                p->key[0] = q->key[j - 1];
                p->branch[0] = r->branch[r->keyTally];
                q->key[j - 1] = r->key[--r->keyTally];
            }
            return;
        }
        if (j<q->keyTally)
        {
            BPlusTree::Node *r = q->branch[j + 1];
            if (r->keyTally>(Max_Number_Of_Branches - 1) >> 1)
            {
                p->key[p->keyTally++] = q->key[j];
                p->branch[p->keyTally] = r->branch[0];
                q->key[j] = r->key[0];
                r->keyTally--;
                for (int i = 0; i<r->keyTally; i++)
                {
                    r->key[i] = r->key[i + 1];
                    r->branch[i] = r->branch[i + 1];
                }
                r->branch[r->keyTally] = r->branch[r->keyTally + 1];
            }
            return;
        }
        if (j<q->keyTally)
        {
            BPlusTree::Node *r = q->branch[j + 1];
            p->key[p->keyTally++] = q->key[j];
            p->branch[p->keyTally] = r->branch[0];
            for (int i = 0; i<r->keyTally; i++)
            {
                p->key[p->keyTally++] = r->key[i];
                p->branch[p->keyTally] = r->branch[i + 1];
            }
            delete r;
            for (int i = j + 1; i<q->keyTally; i++)
            {
                q->key[i - 1] = q->key[i];
                q->branch[i] = q->branch[i + 1];
            }
            q->keyTally--;
            p = q;
            continue;
        }
        BPlusTree::Node *r = q->branch[j - 1];
        r->key[r->keyTally++] = q->key[j - 1];
        r->branch[r->keyTally] = p->branch[0];
        for (int i = 0; i<p->keyTally; i++)
        {
            r->key[r->keyTally++] = p->key[i];
            r->branch[r->keyTally] = p->branch[i + 1];
        }
        delete p;
        for (int i = j; i<q->keyTally; i++)
        {
            q->key[i - 1] = q->key[i];
            q->branch[i] = q->branch[i + 1];
        }
        q->keyTally--;
        p = q;
    }
    return;
}

void Delete(int x, BPlusTree::Node *&root)
{
    BPlusTree::Node *p = FindLeaf(x, root);
    int j;
    for (int i = 0; i<p->keyTally; i++)
        if (p->key[i] == x)
        {
            j = i;
            break;
        }
    for (int i = j; i<p->keyTally - 1; i++)
        p->key[i] = p->key[i + 1];
    p->keyTally--;
    if (p == root || p->keyTally >= (Max_Number_Of_Branches - 1) >> 1)
        return;
    BPlusTree::Node *q = p->father;
    for (int i = 0; i <= q->keyTally; i++)
        if (q->branch[i] == p)
        {
            j = i;
            break;
        }
    if (j>0)
    {
        BPlusTree::Node *r = q->branch[j - 1];
        if (r->keyTally>(Max_Number_Of_Branches - 1) >> 1)
        {
            for (int i = p->keyTally; i>0; i--)
                p->key[i] = p->key[i - 1];
            p->keyTally++;
            p->key[0] = r->key[r->keyTally - 1];
            r->keyTally--;
        }
        Refresh(p->key[0], p);
        return;
    }
    if (j<q->keyTally)
    {
        BPlusTree::Node *r = q->branch[j + 1];
        if (r->keyTally>(Max_Number_Of_Branches - 1) >> 1)
        {
            p->key[p->keyTally++] = r->key[0];
            r->keyTally--;
            for (int i = 0; i<r->keyTally; i++)
                r->key[i] = r->key[i + 1];
        }
        Refresh(r->key[0], r);
        return;
    }
    if (j<q->keyTally)
    {
        BPlusTree::Node *r = q->branch[j + 1];
        for (int i = 0; i<r->keyTally; i++)
            p->key[p->keyTally++] = r->key[i];
        p->Sequential_Next = r->Sequential_Next;
        delete r;
        DeleteIndex(q, j, root);
        return;
    }
    BPlusTree::Node *r = q->branch[j - 1];
    for (int i = 0; i<p->keyTally; i++)
        r->key[r->keyTally++] = p->key[i];
    r->Sequential_Next = p->Sequential_Next;
    delete p;
    DeleteIndex(q, j - 1, root);
    return;
}
