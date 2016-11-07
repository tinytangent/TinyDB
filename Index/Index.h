#ifndef __TINYDB_INDEX_H__
#define __TINYDB_INDEX_H__

class Index
{
public:
    AbstractStorageArea *abstractStorageArea;
    virtual void initializze() = 0;
    virtual void open();
    virtual void close();
    virtual bool lookup();
};

#endif
