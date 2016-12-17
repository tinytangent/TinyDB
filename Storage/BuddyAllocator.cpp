#include "Utils/Config.h"
#include "BuddyAllocator.h"

#include <iostream>
static int integerLog2(int number)
{
    int ret = 0;
    while (number > 1)
    {
        number /= 2;
        ret++;
    }
    return ret;
}

static uint64_t integerPow2(uint64_t number)
{
    return 1 << number;
}

uint64_t BuddyAllocator::getRootBitOffset() const
{
    return 0;
}

uint64_t BuddyAllocator::getParent(uint64_t nodeBitOffset) const
{
    return (nodeBitOffset / 2 - 1) / BIT_PER_NODE * BIT_PER_NODE;
}

uint64_t BuddyAllocator::getLeftChild(uint64_t nodeBitOffset) const
{
    return nodeBitOffset * 2 + BIT_PER_NODE;
}

uint64_t BuddyAllocator::getRightChild(uint64_t nodeBitOffset) const
{
    return nodeBitOffset * 2 + 2 * BIT_PER_NODE;
}

BuddyAllocator::BlockStatus BuddyAllocator::getNodeStatus(uint64_t nodeBitOffset)
{
    uint64_t byte = nodeBitOffset / 8;
    uint64_t bit = nodeBitOffset % 8;
    uint8_t data;
    storageArea->getDataAt(byte, (char*)&data, 1);
    data = (data >> bit) & ((1 << BIT_PER_NODE) - 1);
    return (BlockStatus)data;
}

void BuddyAllocator::setNodeStatusInternal(uint64_t nodeBitOffset, BlockStatus nodeStatus)
{
    uint64_t byte = nodeBitOffset / 8;
    uint64_t bit = nodeBitOffset % 8;
    uint8_t statusValue = (uint8_t)nodeStatus;
    uint8_t data;
    storageArea->getDataAt(byte, (char*)&data, 1);
    data &= ~(((1 << BIT_PER_NODE) - 1) << bit);
    data |= (statusValue << bit);
    storageArea->setDataAt(byte, (char*)&data, 1);
}

void BuddyAllocator::setNodeStatus(uint64_t node, BlockStatus nodeStatus)
{
    auto rootNode = getRootBitOffset();
    setNodeStatusInternal(node, nodeStatus);
    while (node != rootNode)
    {
        node = getParent(node);
        auto prevStatus = getNodeStatus(node);
        BlockStatus nextStatus;
        if (getNodeStatus(getLeftChild(node)) == BlockStatus::FREE &&
            getNodeStatus(getRightChild(node)) == BlockStatus::FREE)
        {
            nextStatus == BlockStatus::FREE;
        }
        else if (
            (getNodeStatus(getLeftChild(node)) == BlockStatus::RESERVED ||
                getNodeStatus(getRightChild(node)) == BlockStatus::ALLOCATED) &&
                (getNodeStatus(getRightChild(node)) == BlockStatus::RESERVED ||
                    getNodeStatus(getRightChild(node)) == BlockStatus::ALLOCATED))
        {
            nextStatus = BlockStatus::ALLOCATED;
        }
        else
        {
            nextStatus = BlockStatus::SPLITTED;
        }
        if (prevStatus != nextStatus)
        {
            std::cout << "STE " << node << std::endl;
            setNodeStatusInternal(node, nextStatus);
        }
        else break;
    }
}

int BuddyAllocator::getOrder(const int size) const
{
    int ret = 0;
    int pow2Size = minOrderSize;
    while (pow2Size < size)
    {
        pow2Size *= 2;
        ret++;
    }
    return ret;
}

uint64_t BuddyAllocator::allocate(uint64_t size, bool isReserved)
{
    uint64_t node = getRootBitOffset();
    int currentOrder = maxOrder;
    int currentOrderSize = chunkSize;
    int targetOrder = getOrder(size);
    if (targetOrder > maxOrder)
        return 0;
    for(;;)
    {
        auto currentOrderStatus = getNodeStatus(node);
        if (currentOrder == targetOrder)
        {
            if (currentOrderStatus != BlockStatus::FREE)
            {
                return 0;
            }
            else
            {
                if (isReserved)
                {
                    setNodeStatus(node, BlockStatus::RESERVED);
                }
                else
                {
                    setNodeStatus(node, BlockStatus::FREE);
                }
                return node2Address(node, targetOrder);
            }
        }
        else
        {
            if (currentOrderStatus == BlockStatus::FREE)
            {
                setNodeStatusInternal(currentOrder, BlockStatus::SPLITTED);
                node = getLeftChild(node);
            }
            else if(currentOrderStatus == BlockStatus::SPLITTED)
            {
                uint64_t leftStatus = getNodeStatus(getLeftChild(node));
                uint64_t rightStatus = getNodeStatus(getRightChild(node));
                if (leftStatus == BlockStatus::FREE || leftStatus == BlockStatus::SPLITTED)
                {
                    node = getLeftChild(node);
                }
                else
                {
                    node = getRightChild(node);
                }
            }
            currentOrder--;
        }
    }
}

uint64_t BuddyAllocator::node2Address(uint64_t nodeBitOffset, int order)
{
    uint64_t orderBegin = (integerPow2(maxOrder - order + 1) - 1) * BIT_PER_NODE;
    return (nodeBitOffset - orderBegin) / BIT_PER_NODE * integerPow2(order) * minOrderSize;
}

bool BuddyAllocator::initialize()
{
    auto zeros = new char[Config::PAGE_SIZE];
    memset(zeros, 0, Config::PAGE_SIZE);
    uint64_t offset = 0;
    while (offset < chunkHeaderSize)
    {
        storageArea->setDataAt(offset, zeros, Config::PAGE_SIZE);
        offset += Config::PAGE_SIZE;
    }
    delete[] zeros;
    allocate(chunkHeaderSize, true);
    return true;
}

uint64_t BuddyAllocator::allocate(uint64_t size)
{
    return allocate(size, false);
}

void BuddyAllocator::free(uint64_t address)
{
    uint64_t orderSequence = address >> integerLog2(minOrderSize);
    uint64_t node = getRootBitOffset();
    for (uint64_t mask = 1 << (maxOrder - 1); mask >= 1; mask >>= 1)
    {
        if ((orderSequence & mask) == 0)
        {
            node = getLeftChild(node);
        }
        else
        {
            node = getRightChild(node);
        }
        if (getNodeStatus(node) == BlockStatus::ALLOCATED &&
            (mask == 1 || getNodeStatus(getLeftChild(node)) == BlockStatus::FREE))
        {
            setNodeStatus(node, BlockStatus::FREE);
            return;
        }
    }
}

BuddyAllocator::BuddyAllocator(AbstractStorageArea * storageArea,
    const int chunkSize, const int minOrderSize)
    :AbstractDynamicAllocator(storageArea),
    chunkSize(chunkSize), minOrderSize(minOrderSize),
    maxOrder(integerLog2(chunkSize/minOrderSize)),
    chunkHeaderSize(chunkSize / minOrderSize * 2 * BIT_PER_NODE / 8)
{
}
