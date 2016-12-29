#pragma once

#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include "AbstractStorageArea.h"

class DiskStorageArea : public AbstractStorageArea
{
protected:
    bool valid = false;
    std::string fileName;
    boost::filesystem::path filePath;
    std::fstream fstream;

    // The current size for fstream object.
    int currentSize = 0;

    // The initial size of data file (16MB)
    const int SIZE_INITITAL = 16 * 1024 * 1024;

    // The ratio for exponential size growing (x2)
    const int SIZE_GROW_RATIO = 2;

    // The size to increase for increaasing data access (256MB)
    const int SIZE_GROW_BYTES = 256 * 1024 * 1024;

    // Max size for exponential size growing (512M)
    const int SIZE_EXPONENTIAL_GROW_LIMIT = 512 * 1024 * 1024;
public:
    DiskStorageArea(std::string fileName);
    virtual bool isValid();
    virtual uint64_t DiskStorageArea::getSize();
    virtual bool checkOffset(uint64_t offset);
    virtual bool setDataAt(int offset, char* data, int length);
    virtual bool getDataAt(int offset, char* data, int length);
    virtual bool close();
    ~DiskStorageArea();
};
