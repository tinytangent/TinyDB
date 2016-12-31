class AbstractStorageArea;
class BlockAllocator;
class FieldType;

class BPlusTree
{
public:
    // Data structure :
    // Parent : 8bytes
    // Used KeyCount : 8 bytes (Only first 4 bytes is used)
    // Keys : keySize * getKeyCount()
    // Branches : 8 * getBranchCount()
    // BranchDatas :  valueSize * getBranchCount()
    // TODO : B+Tree Node doesn't have both child pointer and data.
    class Node
    {
    protected:
        /*Node *branch[Max_Number_Of_Branches];
        int key[Max_Number_Of_Branches - 1], keyTally;
        Node *Sequential_Next, *father;
        bool leaf;*/
    public:
        BPlusTree* bPlusTree;
        uint64_t address;
        Node(BPlusTree* bPlusTree, uint64_t address);
        bool isValid();
        bool isNull();
        Node nullNode();
        Node getParent();
        void setParent(BPlusTree::Node parent);
        Node getNext();
        void setNext(BPlusTree::Node next);
        bool getIsLeaf();
        void setIsLeaf(bool isLeaf);
        int getKeyCount();
        int getBranchCount();
        uint32_t getUsedKeyCount();
        void setUsedKeyCount(uint32_t count);
        Node getBranch(int index);
        bool setBranch(int index, Node node);
        bool getKey(int index, char *buffer);
        bool setKey(int index, char *buffer);
        int insertKey(char * key);
        int findKey(char *buffer);
        int compare(char* data1, char* data2);
        void initialize();
        void internalInsertAfter(char* key, Node left, Node right);
        BPlusTree::Node findLeaf(char *key);
    };
public:
    FieldType * const fieldType;
    AbstractStorageArea *storageArea;
    BlockAllocator *allocator;
    int maxDataPerNode;
    int keySize;
    int valueSize;
    int pageSize;
    int nodeParentOffset;
    int nodeNextOffset;
    int nodeUsedCountOffset;
    int nodeIsLeafOffset;
    int nodeKeysOffset;
    int nodeBranchesOffset;
    int nodeBranchDataOffset;
    Node root;
protected:
    BPlusTree::Node splitLeafNode(BPlusTree::Node fullNode);
    BPlusTree::Node splitInternalNode(BPlusTree::Node fullNode);
    int mergeLeafNode(BPlusTree::Node left, BPlusTree::Node right, int rightIndex);
    int mergeInternalNode(BPlusTree::Node left, BPlusTree::Node right, int rightIndex);
    void balanceLeafNode(BPlusTree::Node left, BPlusTree::Node right, int rightIndex);
    void balanceInternalNode(BPlusTree::Node left, BPlusTree::Node right, int rightIndex);
    void setRootNode(Node newRoot);
public:
    BPlusTree(AbstractStorageArea* storageArea, int keySize, int valueSize, FieldType *fieldType);
    bool initialize();
    bool load();
    Node allocateNode();
	bool freeNode(Node node);
	int insert(int key);// , BPlusTree::Node *root);
	bool checkNodeHalfEmpty(BPlusTree::Node node);
	int Delete(int key);
    int search(int key);
};
