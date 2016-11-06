#ifndef __TINYDB_BIG_INT_FIELD_TYPE_H__
#define __TINYDB_BIG_INT_FIELD_TYPE_H__

#include <cstdint>
#include "FixedLengthIntegerFieldType.h"

class BigIntFieldType : public FixedLengthIntegerFieldType<int64_t>
{
    virtual const std::string getTypeName() const;
    virtual FieldType* construct(ASTNodeBase *astNode);
};

#endif // __TINYDB_BIG_INT_FIELD_TYPE_H__
