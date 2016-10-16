class AbstractFixedAllocator
{
protected:
    AbstractStorageArea* storageArea;
public:
    AbstractFixedAllocator(AbstractStorageArea* storageArea);
    const AbstractStorageArea* getStorageArea() const;
    virtual void initialize() = 0;
    virtual uint64_t bytesTotal() = 0;
    virtual uint64_t bytesUsed() = 0;
    virtual uint64_t bytesAvailable() = 0;
    virtual AbstractStorageArea::AccessProxy allocate() = 0;
    virtual bool free(const AbstractStorageArea::AccessProxy& accessProxy) = 0;
};
