#ifndef __TINYDB_SUFFIX_EXPRESSION_H__
#define __TINYDB_SUFFIX_EXPRESSION_H__

#include <vector>
#include <set>
#include <map>
#include <stack>
#include "Expression/SQLValue.h"

class ExpressionTerm;
class ASTExpression;

class SuffixExpression
{
protected:
    std::vector<ExpressionTerm*> expression;
    std::stack<SQLValue> evaluationStack;
    void construct(const ASTExpression const * astExpression);
public:
    std::set<std::string> requiredContex;
    SuffixExpression(const ASTExpression const * astExpression);
    SQLValue evaluate(std::map<std::string, SQLValue>& context);
};

#endif
