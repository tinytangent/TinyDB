#include <string>
#include <boost/filesystem.hpp>

class Database
{
protected:
    std::string databaseName;
    boost::filesystem::path rootDirectory;
    boost::filesystem::path configFile;
public:
    Database(const std::string& name);
    void setName(const std::string& name);
    std::string getName();
    bool exist();
    bool create();
    bool drop();
};
