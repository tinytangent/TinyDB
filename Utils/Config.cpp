#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include "Config.h"

const int Config::PAGE_SIZE = 8192;

std::string Config::rootDirectory = "";

std::string Config::getRootDirectory()
{
    return rootDirectory;
}

bool Config::setRootDirectory(const std::string& _rootDirectory)
{
    if(!rootDirectory.empty())
    {
        return false;
    }
    rootDirectory = _rootDirectory;
    return true;
}

bool Config::startUpCheck()
{
    boost::filesystem::path dbmsRoot = rootDirectory;
    if(!boost::filesystem::is_directory(dbmsRoot))
    {
        BOOST_LOG_TRIVIAL(fatal) << dbmsRoot << " doesn't exist, or is not a directory.";
        return false;
    }
    return true;
}
