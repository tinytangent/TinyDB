#include "FieldType.h"

std::map<std::string, FieldType*> FieldType::registeredTypes;
bool FieldType::registerType(const std::string &typeName, FieldType *type)
{
    registeredTypes[typeName] = type;
    //TODO: error handling
    return true;
}

FieldType *FieldType::getType(const std::string &typeName)
{
    return registeredTypes[typeName];
}

bool FieldType::isLessThan(char * buffer1, char * buffer2)
{
    return isGreaterThan(buffer2, buffer1);
}

bool FieldType::isNotEqual(char * buffer1, char * buffer2)
{
    return !isEqual(buffer1, buffer2);
}

bool FieldType::isGreaterThanOrEqual(char * buffer1, char * buffer2)
{
    return !isLessThan(buffer1, buffer2);
}

bool FieldType::isLessThanOrEqual(char * buffer1, char * buffer2)
{
    return !isGreaterThan(buffer1, buffer2);
}
