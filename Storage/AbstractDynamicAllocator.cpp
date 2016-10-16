#include "AbstractDynamicAllocator.h"

AbstractDynamicAllocator::AbstractDynamicAllocator(AbstractStorageArea* storageArea, int allocSize)
{
    this->storageArea = storageArea;
    this->allocationSize = allocSize;
}

const AbstractStorageArea* AbstractDynamicAllocator::getStorageArea() const
{
    return storageArea;
}

uint64_t AbstractDynamicAllocator::bytesTotal()
{
    return storageArea->getSize();
}
