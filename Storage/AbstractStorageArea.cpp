#include "AbstractStorageArea.h"

AbstractStorageArea::AccessProxy AbstractStorageArea::operator[] (int offset)
{
    return AccessProxy(this, offset);
}
