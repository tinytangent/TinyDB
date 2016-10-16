#include <cstdint>
#include "AbstractStorageArea.h"

class AbstractDynamicAllocator
{
protected:
    AbstractStorageArea* storageArea;
public:
    AbstractDynamicAllocator(AbstractStorageArea* storageArea);
    const AbstractStorageArea* getStorageArea() const;
    virtual bool initialize() = 0;
    virtual uint64_t bytesTotal();
    virtual uint64_t bytesUsed() = 0;
    virtual uint64_t bytesAvailable() = 0;
    virtual AbstractStorageArea::AccessProxy allocate(uint64_t size) = 0;
    virtual bool free(const AbstractStorageArea::AccessProxy& accessProxy) = 0;
};
