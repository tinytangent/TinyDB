class AbstractStorageArea;

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
        bool setBranchData(int index, char *buffer);
        bool getBranchData(int index, char *buffer);
        int compare(char* data1, char* data2);
        void initialize();
        BPlusTree::Node findLeaf(char *key);
    };
public:
    AbstractStorageArea* storageArea;
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
    Node* root;
    BPlusTree(AbstractStorageArea* storageArea, int keySize, int valueSize);
    int insert(int key, BPlusTree::Node *root);
};
