#include "AbstractDynamicAllocator.h"

AbstractDynamicAllocator::AbstractDynamicAllocator(AbstractStorageArea* storageArea)
{
    this->storageArea = storageArea;
}

AbstractStorageArea* AbstractDynamicAllocator::getStorageArea()
{
    return storageArea;
}

uint64_t AbstractDynamicAllocator::bytesTotal()
{
    return storageArea->getSize();
}
