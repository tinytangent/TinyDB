#include <cstdint>
#include "AbstractStorageArea.h"
#include "AbstractFixedAllocator.h"

class FixedAllocator : public AbstractFixedAllocator
{
protected:
    AbstractStorageArea* storageArea;
    int allocationSize;
    int allocationOffset;
public:
    const int OVERHEAD_SIZE = 8;
    FixedAllocator(AbstractStorageArea* storageArea, int allocSize, int size);
    const AbstractStorageArea* getStorageArea() const;
    virtual void initialize() = 0;
    virtual uint64_t bytesTotal() = 0;
    virtual uint64_t bytesUsed() = 0;
    virtual uint64_t bytesAvailable() = 0;
    virtual AbstractStorageArea::AccessProxy allocate();
    virtual bool free(const AbstractStorageArea::AccessProxy& accessProxy);
};
