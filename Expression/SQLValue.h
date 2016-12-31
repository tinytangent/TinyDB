#ifndef __TINYDB_SQL_VALUE_H__
#define __TINYDB_SQL_VALUE_H__

#include <cstdint>
#include <string>

class SQLValue
{
public:
    enum Type {
        NULL_TYPE,
        INTEGER,
        FLOAT,
        STRING,
        BOOLEAN,
        ERROR,
        NEGATIVE_INFINITY,
        POSITIVE_INFINITY,
    };
    const union {
        int64_t integerValue;
        double floatValue;
        bool boolValue;
    };
    Type type;
    std::string stringValue;
protected:
    SQLValue(Type type);
public:
    SQLValue();
    SQLValue(int64_t intValue);
    SQLValue(double floatValue);
    SQLValue(bool boolValue);
    SQLValue(const std::string& stringValue);
    static SQLValue errorValue();
    static SQLValue negativeInfinity();
    static SQLValue positiveInfinity();
};

#endif
