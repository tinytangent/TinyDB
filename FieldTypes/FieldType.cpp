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
