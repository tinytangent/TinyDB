#include "FixedAllocator.h"

FixedAllocator::FixedAllocator(AbstractStorageArea* storageArea, int unitSize, int allocOffset)
    :AbstractFixedAllocator(storageArea, unitSize + OVERHEAD_SIZE)
{

}

void FixedAllocator::initialize()
{
    uint64_t firstFreeBlockOffset = allocationOffset;
    uint64_t secondFreeBlockOffset = allocationOffset + allocationSize;
    (*storageArea)[firstFreeBlockOffset] = firstFreeBlockOffset;
    (*storageArea)[secondFreeBlockOffset] = firstFreeBlockOffset;
}

uint64_t FixedAllocator::bytesTotal()
{
    //TODO: Unimplemented.
    return 0;
}

uint64_t FixedAllocator::bytesUsed()
{
    //TODO: Unimplemented.
    return 0;
}

uint64_t FixedAllocator::bytesAvailable()
{
    //TODO: Unimplemented
    return 0;
}

AbstractStorageArea::AccessProxy FixedAllocator::allocate()
{
    uint64_t firstFreeBlockOffset = allocationOffset;
    uint64_t nextFreeBlockOffset = (*storageArea)[firstFreeBlockOffset];
    uint64_t nextNextFreeBlockOffset = (*storageArea)[nextFreeBlockOffset];
    if(nextNextFreeBlockOffset != firstFreeBlockOffset)
    {
        (*storageArea)[firstFreeBlockOffset] = nextNextFreeBlockOffset;
    }
    else
    {
        uint64_t newBlockOffset = nextNextFreeBlockOffset + allocationSize;
        (*storageArea)[firstFreeBlockOffset] = newBlockOffset;
        (*storageArea)[newBlockOffset] = (uint64_t)firstFreeBlockOffset;
    }
    return (*storageArea)[nextFreeBlockOffset + OVERHEAD_SIZE];
}

bool FixedAllocator::free(const AbstractStorageArea::AccessProxy& accessProxy)
{
    uint64_t firstFreeBlockOffset = allocationOffset;
    uint64_t secondFreeBlockOffset = (*storageArea)[firstFreeBlockOffset];
    uint64_t freedBlockOffset = accessProxy.getOffset();
    (*storageArea)[firstFreeBlockOffset] = freedBlockOffset;
    (*storageArea)[freedBlockOffset] = secondFreeBlockOffset;
    //TODO: Error handling.
    return true;
}
