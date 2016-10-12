#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>
#include "Utils/Config.h"
#include "Database.h"

Database::Database(const std::string& name)
{
    setName(name);
}

void Database::setName(const std::string& name)
{
    //TODO: Check name so that it doesn't include weird characters.
    this->databaseName = name;
    this->rootDirectory = Config::getRootDirectory();
    this->rootDirectory /= name;
    this->configFile = rootDirectory / ".tinydb.database.json";
}

std::string Database::getName()
{
    return this->databaseName;
}

bool Database::exist()
{
    return boost::filesystem::is_directory(rootDirectory);
}

bool Database::create()
{
    if(exist())
    {
        BOOST_LOG_TRIVIAL(error) <<
            "Trying to create existing database " << databaseName;
        return false;
    }
    if(!boost::filesystem::is_directory(rootDirectory) &&
        !boost::filesystem::create_directory(rootDirectory))
    {
        BOOST_LOG_TRIVIAL(error) <<
            "Cannot create directory " << rootDirectory;
        return false;
    }
    return true;
}

bool Database::drop()
{
    if(!exist())
    {
        BOOST_LOG_TRIVIAL(error) <<
            "Trying to delete non-existing database " << databaseName;
        return false;
    }
    //TODO: Not implemented.
    return false;
}
