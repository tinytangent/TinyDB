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
    };
    const union {
        int64_t integerValue;
        double floatValue;
        bool boolValue;
    };
    Type type;
    std::string stringValue;
    SQLValue();
    SQLValue(int64_t intValue);
    SQLValue(double floatValue);
    SQLValue(bool boolValue);
    SQLValue(const std::string& stringValue);
    static SQLValue errorValue();
};

#endif
