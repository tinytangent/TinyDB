#include <cstdint>

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
            storageArea->getDataAt(offset, &ret, sizeof(T));
            return *(T*)ret;
        }
    };
public:
    virtual bool setDataAt(int offset, char* data, int length) = 0;
    virtual bool getDataAt(int offset, char* data, int length) const = 0;
    virtual uint64_t getSize() = 0;
    AccessProxy operator[] (int offset);
};
