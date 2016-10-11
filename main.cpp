#include <iostream>
#include <string>
#include "Utils/Config.h"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage:TinyDB <rootDirectory>" << std::endl;
        return -1;
    }
    std::string rootDirectory = argv[1];
    Config::setRootDirectory(rootDirectory);
    std::cout << "Welcome to TinyDB!" << std::endl;
    std::cout << "DBMS root directory : " << Config::getRootDirectory()
        << std::endl;
    std::string command;
    std::string dbName;
    for(;;)
    {
        std::cin >> command;
        if(command == "create")
        {
            std::cin >> dbName;
            std::cout << dbName;
        }
        else if(command == "drop")
        {
            std::cin >> dbName;
            std::cout << dbName;
        }
        else if(command == "quit")
        {
            std::cout << "Goodbye." << std::endl;
            break;
        }
    }
    return 0;
}
