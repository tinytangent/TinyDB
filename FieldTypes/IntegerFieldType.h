#include <cstdint>
#include "FixedLengthIntegerFieldType.h"

class IntegerFieldType : public FixedLengthIntegerFieldType<int32_t>
{
    virtual const std::string getTypeName() const;
    virtual FieldType* construct(ASTNodeBase *astNode);
};
