#include "Config.h"

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
