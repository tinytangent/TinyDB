#ifndef __TINYDB_INDEX_H__
#define __TINYDB_INDEX_H__

class Table;
class AbstractStorageArea;

class Index
{
public:
    Table *myTable;
    AbstractStorageArea *abstractStorageArea;
    virtual void initialize() = 0;
    virtual void open() = 0;
    virtual void close() = 0;
    virtual AbstractStorageArea* lookup(char* key) = 0;
};

#endif
