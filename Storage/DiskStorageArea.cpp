#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include "DiskStorageArea.h"

DiskStorageArea::DiskStorageArea(std::string fileName)
{
    this->fileName = fileName;
    filePath = fileName;
    if (!boost::filesystem::is_regular_file(filePath))
    {
        fstream.open(fileName, std::ios::trunc|std::ios::out);
        fstream.close();
        if (!boost::filesystem::is_regular_file(filePath))
        {
            return;
        }
    }
    if(boost::filesystem::file_size(filePath) < SIZE_INITITAL)
    {
        boost::system::error_code err;
        boost::filesystem::resize_file(filePath, SIZE_INITITAL, err);
        if(err != boost::system::errc::success)
        {
            BOOST_LOG_TRIVIAL(error) << "File size allocation failed!";
            return;
        }
    }
    currentSize = boost::filesystem::file_size(filePath);
    fstream.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
    valid = fstream.is_open();
}

uint64_t DiskStorageArea::getSize()
{
    return currentSize;
}

bool DiskStorageArea::isValid()
{
    return valid;
}

bool DiskStorageArea::checkOffset(uint64_t offset)
{
    if(offset < currentSize)
    {
        return true;
    }
    while(offset >= currentSize && currentSize < SIZE_EXPONENTIAL_GROW_LIMIT)
    {
        currentSize *= SIZE_GROW_RATIO;
    }
    while(offset >= currentSize)
    {
        currentSize += SIZE_GROW_BYTES;
    }
    BOOST_LOG_TRIVIAL(debug) <<
        "Trying to extend file " << fileName << " to " <<
        currentSize / 1024 / 1024  << " MB.";
    fstream.close();
    valid = false;
    boost::system::error_code err;
    boost::filesystem::resize_file(filePath, currentSize, err);
    if(err != boost::system::errc::success)
    {
        BOOST_LOG_TRIVIAL(error) << "File extending failed!";
        return false;
    }
    fstream.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
    valid = fstream.is_open();
    if(!valid)
    {
        BOOST_LOG_TRIVIAL(error) << "File extending failed!";
    }
    return valid;
}

bool DiskStorageArea::setDataAt(int offset, char* data, int length)
{
    checkOffset(offset + length);
    fstream.seekp(offset, std::ios_base::beg);
    fstream.write(data, length);
    fstream.flush();
    //TODO: error handling
    return true;
}

bool DiskStorageArea::getDataAt(int offset, char* data, int length)
{
    checkOffset(offset + length);
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
