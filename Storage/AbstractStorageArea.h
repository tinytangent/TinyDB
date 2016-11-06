#ifndef __TINYDB_ABSTRACT_STORAGE_AREA_H__
#define __TINYDB_ABSTRACT_STORAGE_AREA_H__

#include <cstdint>
#include <limits>

class AbstractStorageArea
{
public:
    class AccessProxy
    {
    protected:
        AbstractStorageArea* storageArea;
        int offset;
    public:
        AccessProxy(AbstractStorageArea* storageArea, int offset)
        {
            this->storageArea = storageArea;
            this->offset = offset;
        }

        template<class T>
        const T& operator=(const T& data)
        {
            storageArea->setDataAt(offset, (char*)&data, sizeof(T));
            return data;
        }

        template<class T>
        operator T() const
        {
            char ret[sizeof(T)];
            storageArea->getDataAt(offset, ret, sizeof(T));
            return *(T*)ret;
        }

        uint64_t getOffset() const
        {
            return offset;
        }
    };
public:
    const uint64_t SIZE_UNLIMITED = std::numeric_limits<uint64_t>::max();
    virtual bool isValid() = 0;
    virtual bool setDataAt(int offset, char* data, int length) = 0;
    virtual bool getDataAt(int offset, char* data, int length) = 0;
    virtual uint64_t getSize() = 0;
    virtual ~AbstractStorageArea();
    AccessProxy operator[] (int offset);
};

#endif