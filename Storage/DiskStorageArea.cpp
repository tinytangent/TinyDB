#include <boost/filesystem.hpp>
#include "DiskStorageArea.h"

DiskStorageArea::DiskStorageArea(std::string fileName)
{
    boost::filesystem::path filePath = fileName;
    if (!boost::filesystem::is_regular_file(filePath))
    {
        fstream.open(fileName, std::ios::trunc|std::ios::out);
        fstream.close();
        if (!boost::filesystem::is_regular_file(filePath))
        {
            return;
        }
    }
    if(boost::filesystem::file_size(filePath) < FILE_SIZE)
    {
        boost::system::error_code err;
        boost::filesystem::resize_file(filePath, FILE_SIZE, err);
        if(err != boost::system::errc::success)
        {
            return;
        }
    }
    fstream.open(fileName);
    valid = fstream.is_open();
}

uint64_t DiskStorageArea::getSize()
{
    return FILE_SIZE;
}

bool DiskStorageArea::isValid()
{
    return valid;
}

bool DiskStorageArea::setDataAt(int offset, char* data, int length)
{
    fstream.seekp(offset, std::ios_base::beg);
    fstream.write(data, length);
    //TODO: error handling
    return true;
}

bool DiskStorageArea::getDataAt(int offset, char* data, int length)
{
    fstream.seekg(offset, std::ios_base::beg);
    fstream.read(data, length);
    //TODO: error handling
    return true;
}

bool DiskStorageArea::close()
{
    if(!isValid()) {
        return false;
    }
    fstream.close();
    valid = false;
    return true;
}

DiskStorageArea::~DiskStorageArea()
{
    if(fstream.is_open())
    {
        fstream.close();
    }
}
