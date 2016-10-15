#include <string>
#include <fstream>
#include "AbstractStorageArea.h"

class DiskStorageArea : public AbstractStorageArea
{
protected:
    bool valid = false;
    std::string fileName;
    std::fstream fstream;
    const int FILE_SIZE = 1024 * 1024 * 1024;
public:
    DiskStorageArea(std::string fileName);
    virtual bool isValid();
    virtual uint64_t DiskStorageArea::getSize();
    virtual bool setDataAt(int offset, char* data, int length);
    virtual bool getDataAt(int offset, char* data, int length);
    virtual bool close();
    ~DiskStorageArea();
};
