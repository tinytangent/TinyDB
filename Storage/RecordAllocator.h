#ifndef __TINYDB_RECORD_ALLOCATOR_H__
#define __TINYDB_RECORD_ALLOCATOR_H__

#include <cstdint>
#include "AbstractStorageArea.h"
#include "AbstractFixedAllocator.h"

class RecordAllocator : public AbstractFixedAllocator
{
protected:
    int allocationOffset;
    int blockSize;
    int recordBitmapSize;
public:
    const int OVERHEAD_SIZE = 8;
    RecordAllocator(AbstractStorageArea* storageArea, int recordSize);
    //const AbstractStorageArea* getStorageArea() const;
    virtual void initialize();
    virtual void initializeBlock(uint64_t blockOffset);
    virtual int getBlockFreeSlot(uint64_t blockOffset);
    virtual uint64_t bytesTotal();
    virtual uint64_t bytesUsed();
    virtual uint64_t bytesAvailable();
    virtual AbstractStorageArea::AccessProxy allocate();
    virtual bool free(const AbstractStorageArea::AccessProxy& accessProxy);
};

#endif // __TINYDB_RECORD_ALLOCATOR_H__
