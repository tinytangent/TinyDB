#include <cstdint>

#include "AbstractDynamicAllocator.h"

class BuddyAllocator : public AbstractDynamicAllocator
{
protected:
    const int BIT_PER_NODE = 2;
    enum BlockStatus{
        FREE = 0,
        SPLITTED = 1,
        ALLOCATED = 2,
        RESERVED = 3,
    };
    uint64_t getRootBitOffset() const;
    uint64_t getParent(uint64_t nodeBitOffset) const;
    uint64_t getLeftChild(uint64_t nodeBitOffset) const;
    uint64_t getRightChild(uint64_t nodeBitOffset) const;
    BlockStatus getNodeStatus(uint64_t nodeBitOffset);
    void setNodeStatusInternal(uint64_t nodeBitOffset, BlockStatus nodeStatus);
    void setNodeStatus(uint64_t nodeBitOffset, BlockStatus nodeStatus);
    int getOrder(const int size) const;
    uint64_t allocate(uint64_t size, bool isReserved = false);
    uint64_t node2Address(uint64_t nodeBitOffset, int order);
public:
    //"Chunk" is a large area on the file managed by the buddy allocator.
    //A chunk is usually in size like 512MB/1GB. Note that the max possible
    //memory can be allocated is chunkSize / 2.
    const int chunkSize;

    //The minimum size to alllocate each time
    const int minOrderSize;

    //The max order
    const int maxOrder;

    const int chunkHeaderSize;
    bool initialize() override;
    uint64_t allocate(uint64_t size) override;
    void free(uint64_t address) override;
    BuddyAllocator(AbstractStorageArea *storageArea, const int chunkSize, const int minOrderSize);
};
