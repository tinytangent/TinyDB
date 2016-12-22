#include <iostream>
#include "BlockAllocator.h"

BlockAllocator::BlockAllocator(AbstractStorageArea* storageArea)
    :AbstractFixedAllocator(storageArea, 8192)
{
    allocationOffset = 8 * 8192;
    blockStatusBitmapOffset = 4 * 8192;
    blockSize = 8192;
    blocksPerMacroBlock = 1024 * 1024 / 8;
    blocksReservedPerMacroBlock = 8;
}

void BlockAllocator::initialize()
{
    //Construct link list with a loop, where they first block after
    //allocation offset is always reserved.
    uint64_t metaBlockOffset = allocationOffset;
    uint64_t firstFreeBlockOffset = metaBlockOffset + blockSize;
    uint64_t secondFreeBlockOffset = firstFreeBlockOffset + blockSize;
    (*storageArea)[metaBlockOffset] = firstFreeBlockOffset;
    (*storageArea)[firstFreeBlockOffset] = secondFreeBlockOffset;
    (*storageArea)[secondFreeBlockOffset] = metaBlockOffset;
    initializeMacroBlock(0);
}

uint64_t BlockAllocator::getMacroBlockOffset(uint64_t macroBlockIndex)
{
    return macroBlockIndex * macroBlockSize;
}

void BlockAllocator::initializeMacroBlock(uint64_t macroBlockOffset)
{
    for (int i = 0; i < blocksPerMacroBlock; i++)
    {
        if (i < blocksReservedPerMacroBlock)
        {
            setBlockStatus(macroBlockOffset, i, BlockStatus::RESERVED);
        }
        else
        {
            setBlockStatus(macroBlockOffset, i, BlockStatus::UNUSED);
        }
    }
}

bool BlockAllocator::setBlockStatus(uint64_t macroBlockOffset, uint64_t blockIndex, BlockStatus status)
{
    int blockStatusBit = blockIndex * 2;
    int blockStatusByte = blockStatusBit / 8;
    blockStatusBit %= 8;
    uint8_t data;
    storageArea->getDataAt(macroBlockOffset + blockStatusBitmapOffset + blockStatusByte, (char*)&data, 1);
    data &= ~(0x03 << blockStatusBit);
    uint8_t new_flag = status;
    data |= new_flag << blockStatusBit;
    storageArea->setDataAt(macroBlockOffset + blockStatusBitmapOffset + blockStatusByte, (char*)&data, 1);
    return true;
}

BlockAllocator::BlockStatus BlockAllocator::getBlockStatus(uint64_t macroBlockOffset, uint64_t blockIndex)
{
    int blockStatusBit = blockIndex * 2;
    int blockStatusByte = blockStatusBit / 8;
    blockStatusBit %= 8;
    uint8_t data;
    storageArea->getDataAt(macroBlockOffset + blockStatusBitmapOffset + blockStatusByte, (char*)&data, 1);
    data = (data >> blockStatusBit) & 0x03;
    return (BlockStatus)data;
}

uint64_t BlockAllocator::getBlockOffset(uint64_t macroBlockOffset, uint64_t blockIndex)
{
    return macroBlockOffset + blockSize * blockIndex;// +allocationOffset;
}

uint64_t BlockAllocator::bytesTotal()
{
    //TODO: Unimplemented.
    return 0;
}

uint64_t BlockAllocator::bytesUsed()
{
    //TODO: Unimplemented.
    return 0;
}

uint64_t BlockAllocator::bytesAvailable()
{
    //TODO: Unimplemented
    return 0;
}

AbstractStorageArea::AccessProxy BlockAllocator::allocate()
{
    for (int i = 0; i < blocksPerMacroBlock; i++)
    {
        if (getBlockStatus(0, i) == BlockStatus::UNUSED)
        {
            setBlockStatus(0, i, BlockStatus::FULLY_USED);
            return (*storageArea)[getBlockOffset(0, i)];
        }
    }
    return (*storageArea)[0];
}

bool BlockAllocator::free(const AbstractStorageArea::AccessProxy& accessProxy)
{
    uint64_t blockIndex = accessProxy.getOffset() / blockSize;
    setBlockStatus(0, blockIndex, BlockStatus::UNUSED);
    return true;
}
