#include "AbstractDynamicAllocator.h"

AbstractDynamicAllocator::AbstractDynamicAllocator(AbstractStorageArea* storageArea)
{
    this->storageArea = storageArea;
}

const AbstractStorageArea* AbstractDynamicAllocator::getStorageArea() const
{
    return storageArea;
}

uint64_t AbstractDynamicAllocator::bytesTotal()
{
    return storageArea->getSize();
}
