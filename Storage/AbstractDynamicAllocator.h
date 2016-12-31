#include <cstdint>
#include "AbstractStorageArea.h"

class AbstractDynamicAllocator
{
protected:
    AbstractStorageArea* storageArea;
public:
    AbstractDynamicAllocator(AbstractStorageArea* storageArea);
    AbstractStorageArea* getStorageArea();
    virtual bool initialize() = 0;
    virtual uint64_t bytesTotal();
    //virtual uint64_t bytesUsed() = 0;
    //virtual uint64_t bytesAvailable() = 0;
    virtual uint64_t allocate(uint64_t size) = 0;
    virtual bool free(uint64_t address, uint64_t size) = 0;
};
