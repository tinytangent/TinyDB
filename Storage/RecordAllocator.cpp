#include "RecordAllocator.h"

RecordAllocator::RecordAllocator(AbstractStorageArea* storageArea, int recordSize)
    :AbstractFixedAllocator(storageArea, recordSize)
{
    allocationOffset = 8 * 8192;
    blockStatusBitmapOffset = 4 * 8192;
    blockSize = 8192;
    recordBitmapSize = 128;
    blocksPerMacroBlock = 1024 * 1024 / 8;
    blocksReservedPerMacroBlock = 8;
}

void RecordAllocator::initialize()
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
    initializeBlock(firstFreeBlockOffset);
}

uint64_t RecordAllocator::getMacroBlockOffset(uint64_t macroBlockIndex)
{
    return macroBlockIndex * macroBlockSize;
}

void RecordAllocator::initializeMacroBlock(uint64_t macroBlockOffset)
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

bool RecordAllocator::setBlockStatus(uint64_t macroBlockOffset, uint64_t blockIndex, BlockStatus status)
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

RecordAllocator::BlockStatus RecordAllocator::getBlockStatus(uint64_t macroBlockOffset, uint64_t blockIndex)
{
    int blockStatusBit = blockIndex * 2;
    int blockStatusByte = blockStatusBit / 8;
    blockStatusBit %= 8;
    uint8_t data;
    storageArea->getDataAt(macroBlockOffset + blockStatusBitmapOffset + blockStatusByte, (char*)&data, 1);
    data = (data >> blockStatusBit) & 0x03;
    return (BlockStatus)data;
}

uint64_t RecordAllocator::getBlockOffset(uint64_t macroBlockOffset, uint64_t blockIndex)
{
    return macroBlockOffset + blockSize * blockIndex + allocationOffset;
}

void RecordAllocator::initializeBlock(uint64_t blockOffset)
{
    //TODO: assert on block offset
    int arraySize = recordBitmapSize / 8;
    int recordSize = allocationSize;
    //TODO: This can be optimized by pre-allocating
    uint64_t *recordBitmap = new uint64_t[arraySize];
    for(int i = 0; i < arraySize; i++)
    {
        recordBitmap[i] = ~(uint64_t)0;
    }
    int capacity = (blockSize - 8 - recordBitmapSize) / recordSize;
    for(int i = 0; i < capacity; i++)
    {
        //TODO: This can be optimized.
        int index = i / 64;
        int bit = i % 64;
        recordBitmap[index] &= ~(((uint64_t)1) << bit);
    }
    storageArea->setDataAt(blockOffset + 8, (char*)recordBitmap, recordBitmapSize);
    delete[] recordBitmap;
}

bool RecordAllocator::blockIsRecordUsed(uint64_t blockOffset, int recordPos)
{
    int arraySize = recordBitmapSize / 8;
    uint64_t *recordBitmap = new uint64_t[arraySize];
    int index = recordPos / 64;
    int bit = recordPos % 64;
    storageArea->getDataAt(blockOffset + 8, (char*)recordBitmap, recordBitmapSize);
    bool ret = ((recordBitmap[index] >> bit) & (uint64_t)0x1) == 1;
    delete recordBitmap;
    return ret;
}

uint64_t RecordAllocator::recordGetBlockOffset(uint64_t blockOffset, int recordPos)
{
    return blockOffset + 8 + recordBitmapSize + recordPos * allocationSize;
}

int RecordAllocator::getBlockFreeSlot(uint64_t blockOffset)
{
    //TODO: assert on block offset
    int arraySize = recordBitmapSize / 8;
    //TODO: This can be optimized by pre-allocating
    uint64_t *recordBitmap = new uint64_t[arraySize];
    storageArea->getDataAt(blockOffset + 8, (char*)recordBitmap, recordBitmapSize);
    for(int i = 0; i < arraySize; i++)
    {
        if(recordBitmap[i] == ~(uint64_t)0) continue;
        for(int j = 0; j < 64; j++)
        {
            if(((recordBitmap[i] >> j) & (uint64_t)0x1) == 0)
            {
                delete[] recordBitmap;
                return i * 64 + j;
            }
        }
    }
    delete[] recordBitmap;
    return -1;
}

uint64_t RecordAllocator::bytesTotal()
{
    //TODO: Unimplemented.
    return 0;
}

uint64_t RecordAllocator::bytesUsed()
{
    //TODO: Unimplemented.
    return 0;
}

uint64_t RecordAllocator::bytesAvailable()
{
    //TODO: Unimplemented
    return 0;
}

AbstractStorageArea::AccessProxy RecordAllocator::allocate()
{
    uint64_t metaBlockOffset = allocationOffset;
    uint64_t firstFreeBlockOffset, secondFreeBlockOffset, thirdFreeBlockOffset;
    storageArea->getDataAt(metaBlockOffset, (char*)&firstFreeBlockOffset, sizeof(uint64_t));
    int freeRecordPosition = getBlockFreeSlot(firstFreeBlockOffset);
    //TODO: Assert this is not -1
    int index = freeRecordPosition / 64;
    int bit = freeRecordPosition % 64;
    uint64_t temp;
    uint64_t modifiedAddress = firstFreeBlockOffset + 8 + sizeof(uint64_t) * index;
    storageArea->getDataAt(modifiedAddress, (char*)&temp, sizeof(temp));
    temp |= ((uint64_t)1) << bit;
    storageArea->setDataAt(modifiedAddress, (char*)&temp, sizeof(temp));
    if(getBlockFreeSlot(firstFreeBlockOffset) == -1)
    {
        storageArea->getDataAt(firstFreeBlockOffset, (char*)&secondFreeBlockOffset, sizeof(uint64_t));
        storageArea->getDataAt(secondFreeBlockOffset, (char*)&thirdFreeBlockOffset, sizeof(uint64_t));
        if(thirdFreeBlockOffset != metaBlockOffset)
        {
            (*storageArea)[metaBlockOffset] = secondFreeBlockOffset;
            initializeBlock(secondFreeBlockOffset);
        }
        else
        {
            uint64_t newBlockOffset = secondFreeBlockOffset + blockSize;
            (*storageArea)[metaBlockOffset] = secondFreeBlockOffset;
            (*storageArea)[secondFreeBlockOffset] = newBlockOffset;
            (*storageArea)[newBlockOffset] = (uint64_t)metaBlockOffset;
            initializeBlock(secondFreeBlockOffset);
        }
    }
    return (*storageArea)[firstFreeBlockOffset + OVERHEAD_SIZE + 
        recordBitmapSize + freeRecordPosition * allocationSize];
}

bool RecordAllocator::free(const AbstractStorageArea::AccessProxy& accessProxy)
{
    //TODO: Needs to be rewritten.
    /*uint64_t firstFreeBlockOffset = allocationOffset;
    uint64_t secondFreeBlockOffset = (*storageArea)[firstFreeBlockOffset];
    uint64_t freedBlockOffset = accessProxy.getOffset();
    (*storageArea)[firstFreeBlockOffset] = freedBlockOffset;
    (*storageArea)[freedBlockOffset] = secondFreeBlockOffset;*/
    //TODO: Error handling.
    return true;
}
