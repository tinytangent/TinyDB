#include "AbstractStorageArea.h"
#include "AbstractFixedAllocator.h"

AbstractFixedAllocator::AbstractFixedAllocator(AbstractStorageArea* storageArea)
{
    this->storageArea = storageArea;
}

const AbstractStorageArea* AbstractFixedAllocator::getStorageArea() const
{
    return storageArea;
}