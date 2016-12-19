#include "SQLValue.h"

SQLValue::SQLValue()
{

}

SQLValue::SQLValue(int64_t intValue)
{
    this->integerValue = intValue;
    type = INTEGER;
}

SQLValue::SQLValue(double floatValue)
{
    type = FLOAT;
    this->floatValue = floatValue;
}

SQLValue::SQLValue(const std::string & stringValue)
{
    type = STRING;
    this->stringValue = stringValue;
}
