#include <iostream>
#include <string>
#include "Utils/Config.h"
#include "Database/Database.h"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage:TinyDB <rootDirectory>" << std::endl;
        return -1;
    }
    std::string rootDirectory = argv[1];
    std::cout << "Welcome to TinyDB!" << std::endl;
    Config::setRootDirectory(rootDirectory);
    if(!Config::startUpCheck())
    {
        return -1;
    }
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
            Database database(dbName);
            database.create();
        }
        else if(command == "drop")
        {
            std::cin >> dbName;
            Database database(dbName);
            database.drop();
        }
        else if(command == "quit")
        {
            std::cout << "Goodbye." << std::endl;
            break;
        }
    }
    return 0;
}
