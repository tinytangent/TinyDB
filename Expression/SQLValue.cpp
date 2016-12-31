#include "SQLValue.h"

SQLValue::SQLValue(Type type)
    :type(type)
{

}

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

SQLValue::SQLValue(bool boolValue)
{
    type = BOOLEAN;
    this->boolValue = boolValue;
}

SQLValue::SQLValue(const std::string & stringValue)
{
    type = STRING;
    this->stringValue = stringValue;
}

SQLValue SQLValue::errorValue()
{
    return SQLValue(ERROR);
}

SQLValue SQLValue::negativeInfinity()
{
    return SQLValue(NEGATIVE_INFINITY);
}

SQLValue SQLValue::positiveInfinity()
{
    return SQLValue(POSITIVE_INFINITY);
}