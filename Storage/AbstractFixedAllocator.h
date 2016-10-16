class AbstractFixedAllocator
{
protected:
    AbstractStorageArea* storageArea;
    int allocationSize;
public:
    AbstractFixedAllocator(AbstractStorageArea* storageArea, int allocSize);
    const AbstractStorageArea* getStorageArea() const;
    virtual void initialize() = 0;
    virtual uint64_t bytesTotal() = 0;
    virtual uint64_t bytesUsed() = 0;
    virtual uint64_t bytesAvailable() = 0;
    virtual AbstractStorageArea::AccessProxy allocate() = 0;
    virtual bool free(const AbstractStorageArea::AccessProxy& accessProxy) = 0;
};
