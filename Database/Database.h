#ifndef __TINYDB_DATABASE_H__
#define __TINYDB_DATABASE_H__

#include <string>
#include <map>
#include <boost/filesystem.hpp>

class ASTCreateTableStmtNode;
class CachedStorageArea;
class AbstractDynamicAllocator;
class Table;
class Index;
class Constraint;

class Database
{
protected:
    std::string databaseName;
    boost::filesystem::path rootDirectory;
    boost::filesystem::path configFilePath;
    std::map<std::string, Table*> tables;
    std::map<std::string, Index*> indexes;
    std::map<std::string, Constraint*> constraints;
    bool isOpened;
protected:

    /**
     * Load the database config file (metadata.tinydb.json)
     */
    bool loadConfigFile();

    /**
     * Save the database config file (metadata.tinydb.json)
     */
    bool saveConfigFile();

    /**
     * Create 2 storage files (fixed/dynamic area) for a new table.
     * Those files won't be initialized.
     * This is a internal method used by createTable().
     * @param tableName : The name of the new table.
     * @ret whether table files creation succeeded.
     */
    bool createTableFile(const std::string& tableName);

    /**
    * Delete storage file related to a table.
    * @param tableName : The name of the table to be deleted.
    * @ret whether table deletion succeeded.
    */
    bool deleteTableFile(const std::string& tableName);
public:
    Database(const std::string& name, const boost::filesystem::path& path);
    void setName(const std::string& name);
    std::string getName();
    bool exist();
    bool create();
    bool drop();
    bool open();
    bool close();
    /**
     * Create a new table with the information given by astNode
     * @param astNode : The AST Node providing information
     * about database creation.
     * @ret whether table creation succeeded.
     */
    bool createTable(ASTCreateTableStmtNode* astNode);

    Table* getTable(const std::string& tableName);

    std::map<std::string, Table*>& getAllTables();

    bool dropTable(const std::string& tableName);

    // Index operations

    bool createIndex(const std::string& indexName, const std::string& tableName, const std::string& columnName);
    Index* getIndex(const std::string& indexName);
    std::map<std::string, Index*>& getAllIndexes();
    bool dropIndex(const std::string& indexName);

    // Constraint operations

    bool addConstraint(const std::string& constraintName, Constraint * constraint);
    Constraint* getConstraint(const std::string& constraintName);
    std::map<std::string, Constraint*>& getAllConstraints();
    bool dropConstraint(const std::string& constraintName);
};

#endif
