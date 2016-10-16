#include <boost/log/trivial.hpp>
#include "CachedStorageArea.h"

CachedStorageArea::CachedStorageArea(std::string fileName, int cacheSize, int cacheBlockSize)
    :DiskStorageArea(fileName)
{
    this->cacheBlockSize = cacheBlockSize;
    cachedBlocks = cacheSize / cacheBlockSize;
    dirtyFlags.resize(cachedBlocks);
    cacheBlockBit = 0;
    for(int i = 1; i < 32; i++)
    {
        if(cacheBlockSize == (1 << i))
        {
            cacheBlockBit = i;
        }
    }
    if(cacheBlockBit == 0)
    {
        BOOST_LOG_TRIVIAL(fatal)
            << "Unaligned cacheBlockSize for CachedStorageArea!";
    }
    cacheBlockMask = cacheBlockSize - 1;
}

bool CachedStorageArea::access(uint64_t offset)
{
    uint64_t blockIndex = getBlock(offset);
    if(cacheMapping.find(blockIndex) != cacheMapping.end()) {
        cacheHistory.push_front(blockIndex);
        cacheHistory.erase(cacheHistoryMapping[blockIndex]);
        cacheHistoryMapping[blockIndex] = cacheHistory.begin();
        return true;
    }
    accessHistory.push_front(blockIndex);
    accessRecord[blockIndex]++;
    if(accessHistory.size() > HISTORY_SIZE)
    {
        uint64_t historyToDrop = accessHistory.back();
        accessRecord[historyToDrop]--;
        if(accessRecord[historyToDrop] == 0)
        {
            accessRecord.erase(historyToDrop);
        }
    }
    if(accessRecord[blockIndex] >= LRU_ACCESS_TIMES)
    {
        addToCache(blockIndex);
    }
    return true;
}

uint64_t CachedStorageArea::getBlock(uint64_t offset)
{
    return offset >> cacheBlockBit;
}

uint64_t CachedStorageArea::translateAddress(uint64_t address)
{
    int offset = address & cacheBlockMask;
    return (cacheMapping[getBlock(offset)] << cacheBlockBit) + offset;
}

void CachedStorageArea::addToCache(uint64_t blockIndex)
{
    uint64_t blockToRemoveOrig = cacheHistory.back();
    uint64_t blockToRemoveCached = cacheMapping[blockToRemoveOrig];
    if(cacheHistory.size() >= cachedBlocks)
    {
        if(dirtyFlags[blockToRemoveCached])
        {
            // TODO: For better performance, use bit operation.
            DiskStorageArea::setDataAt(
                blockToRemoveOrig * cacheBlockSize,
                cache + blockToRemoveCached * cacheBlockSize,
                cacheBlockSize
            );
            dirtyFlags[blockToRemoveCached] = false;
        }
        cacheHistory.pop_back();
        cacheHistoryMapping.erase(blockToRemoveOrig);
        cacheMapping.erase(blockToRemoveOrig);
    }
    uint64_t blockToAddOrig = blockIndex;
    uint64_t blockToAddCached = blockToRemoveCached;
    cacheHistory.push_front(blockToAddOrig);
    cacheHistoryMapping[blockToAddOrig] = cacheHistory.begin();
    cacheMapping[blockToAddOrig] = blockToAddCached;
    DiskStorageArea::getDataAt(
        blockToAddOrig * cacheBlockSize,
        cache + blockToAddCached * cacheBlockSize,
        cacheBlockSize
    );
}

bool CachedStorageArea::singleBlockGetDataAt(int offset, char* data, int length)
{
    if(!access(offset))
    {
        return DiskStorageArea::getDataAt(offset, data, length);
    }
    memcpy(data, cache + translateAddress(offset), length);
    return true;
}

bool CachedStorageArea::singleBlockSetDataAt(int offset, char* data, int length)
{
    if(!access(offset))
    {
        return DiskStorageArea::setDataAt(offset, data, length);
    }
    uint64_t translatedAddress = translateAddress(offset);
    memcpy(cache + translatedAddress, data, length);
    dirtyFlags[translatedAddress >> cacheBlockBit] = true;
    return true;
}

bool CachedStorageArea::setDataAt(int offset, char* data, int length)
{
    //TODO: handles access across cache-blocks.
    return singleBlockSetDataAt(offset, data, length);
}

bool CachedStorageArea::getDataAt(int offset, char* data, int length)
{
    //TODO: handles access across cache-blocks;
    return singleBlockGetDataAt(offset, data, length);
}
