#include "Index.h"

Index::Index(Database * database, const std::string & tableName, const std::string & columnName, const boost::filesystem::path & storageFileName)
    :database(database), tableName(tableName), columnName(columnName), storagePath(storageFileName)
{
}
