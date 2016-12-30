#ifndef __TINYDB_STORAGE_CACHED_STIRAGE_AREA_H__
#define __TINYDB_STORAGE_CACHED_STIRAGE_AREA_H__

#include <map>
#include <list>
#include <vector>
#include <cstdint>
#include "DiskStorageArea.h"

class CachedStorageArea : public DiskStorageArea
{
protected:
    uint64_t lastCachedBlockIndex = ~(uint64_t)0;
    uint64_t lastTranslatedAddress = ~(uint64_t)0;
    std::map<uint64_t, int> accessRecord;
    std::list<uint64_t> accessHistory;
    std::map<uint64_t, uint64_t> cacheMapping;
    std::map<uint64_t, std::list<uint64_t>::iterator> cacheHistoryMapping;
    std::list<uint64_t> cacheHistory;
    int cachedBlocks;
    int cacheBlockSize;
    int cacheBlockBit;
    uint64_t cacheBlockMask;
    char* cache;
    std::vector<bool> dirtyFlags;
    virtual bool singleBlockGetDataAt(int offset, char* data, int length);
    virtual bool singleBlockSetDataAt(int offset, char* data, int length);
    virtual void addToCache(uint64_t blockIndex);
    virtual uint64_t translateAddress(uint64_t address);
    virtual uint64_t getBlock(uint64_t offset);
public:
    const int HISTORY_SIZE = 256;
    const int LRU_ACCESS_TIMES = 2;
    CachedStorageArea(std::string fileName, int cacheSize, int cacheBlockSize);
    virtual bool access(uint64_t offset);
    virtual bool setDataAt(int offset, char* data, int length);
    virtual bool getDataAt(int offset, char* data, int length);
    virtual void flush();
};

#endif