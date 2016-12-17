#include <string>
#include <boost/filesystem.hpp>

class Config
{
protected:
    static std::string rootDirectory;
public:
    static std::string getRootDirectory();
    static bool setRootDirectory(const std::string& rootDirectory);
    static bool startUpCheck();
    const static int PAGE_SIZE;
};
