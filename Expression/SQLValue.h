#ifndef __TINYDB_SQL_VALUE_H__
#define __TINYDB_SQL_VALUE_H__

#include <cstdint>
#include <string>

class SQLValue
{
public:
    enum Type {
        INTEGER,
        FLOAT,
        STRING,
    };
    const union {
        int64_t integerValue;
        double floatValue;
    };
    Type type;
    std::string stringValue;
    SQLValue();
    SQLValue(int64_t intValue);
    SQLValue(double floatValue);
    SQLValue(const std::string& stringValue);
};

#endif