#include <string>

class Config
{
protected:
    static std::string rootDirectory;
public:
    static std::string getRootDirectory();
    static bool setRootDirectory(const std::string& rootDirectory);
};
