#include "RecordAllocator.h"

RecordAllocator::RecordAllocator(AbstractStorageArea* storageArea, int recordSize)
    :AbstractFixedAllocator(storageArea, recordSize)
{
    allocationOffset = 4096;
    blockSize = 4096;
    recordBitmapSize = 128;
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
    initializeBlock(firstFreeBlockOffset);
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
