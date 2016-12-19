#ifndef __TINYDB_EXPRESSION_TERM_H__
#define __TINYDB_EXPRESSION_TERM_H__

#include "Parser/ASTNodes.h"
#include <string>

class ExpressionTerm
{
public:
    enum Type
    {
        INTEGER,
        STRING,
        COLUMN_NAME,
        OPERATOR
    };
protected:
    ExpressionTerm();
public:
    Type type;
    std::string stringValue;
    bool isValidInteger;
    int64_t intValue;
    ASTExpression::Operator op;
    static ExpressionTerm *createIntegerValueTerm(int64_t value);
    static ExpressionTerm *createStringValueTerm(const std::string& val);
    static ExpressionTerm *createColumnNameTerm(const std::string& column);
    static ExpressionTerm *createColumnOperatorTerm(ASTExpression::Operator op);
};

#endif
