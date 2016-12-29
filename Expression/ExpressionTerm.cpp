#include "ExpressionTerm.h"

ExpressionTerm::ExpressionTerm()
{
}

ExpressionTerm * ExpressionTerm::createIntegerValueTerm(int64_t value)
{
    ExpressionTerm *ret = new ExpressionTerm();
    ret->type = INTEGER;
    ret->intValue = value;
    ret->stringValue = std::to_string(value);
    ret->isValidInteger = true;
    return ret;
}

ExpressionTerm * ExpressionTerm::createBooleanValueTerm(bool value)
{
    ExpressionTerm *ret = new ExpressionTerm();
    ret->type = BOOLEAN;
    ret->boolValue = value;
    ret->isValidInteger = true;
    return ret;
}

ExpressionTerm * ExpressionTerm::createStringValueTerm(const std::string& value)
{
    ExpressionTerm *ret = new ExpressionTerm();
    ret->type = STRING;
    ret->intValue = std::stoi("0" + value);
    ret->stringValue = value;
    ret->isValidInteger = true;
    return ret;
}

ExpressionTerm * ExpressionTerm::createColumnNameTerm(const std::string& column)
{
    ExpressionTerm *ret = new ExpressionTerm();
    ret->type = COLUMN_NAME;
    ret->stringValue = column;
    ret->isValidInteger = false;
    return ret;
}

ExpressionTerm * ExpressionTerm::createColumnOperatorTerm(ASTExpression::Operator op)
{
    ExpressionTerm *ret = new ExpressionTerm();
    ret->type = OPERATOR;
    ret->op = op;
    ret->isValidInteger = false;
    return ret;
}
