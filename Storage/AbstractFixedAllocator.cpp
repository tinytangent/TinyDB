#include "AbstractStorageArea.h"
#include "AbstractFixedAllocator.h"

AbstractFixedAllocator::AbstractFixedAllocator(AbstractStorageArea* storageArea, int allocSize)
{
    this->storageArea = storageArea;
    this->allocationSize = allocSize;
}

const AbstractStorageArea* AbstractFixedAllocator::getStorageArea() const
{
    return storageArea;
}
