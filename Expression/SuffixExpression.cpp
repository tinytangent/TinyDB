#include <boost/log/trivial.hpp>

#include "Parser/ASTNodes.h"
#include "ExpressionTerm.h"
#include "SuffixExpression.h"

SuffixExpression::SuffixExpression(const ASTExpression const * astExpression)
{
    construct(astExpression);
}

SQLValue SuffixExpression::evaluate(std::map<std::string, SQLValue>& context)
{
    while(!evaluationStack.empty())
        evaluationStack.pop();
    for (int i = 0; i < expression.size(); i++)
    {
        auto& term = expression[i];
        if (term->type == term->OPERATOR)
        {
            SQLValue val2 = evaluationStack.top();
            evaluationStack.pop();
            SQLValue val1 = evaluationStack.top();
            evaluationStack.pop();
            if (val1.type != SQLValue::INTEGER || val2.type != SQLValue::INTEGER)
            {
                BOOST_LOG_TRIVIAL(error) <<
                    "Currently only integer operation is supported.";
            }
            auto integer1 = val1.integerValue;
            auto integer2 = val2.integerValue;
            decltype(integer1) result;
            switch (term->op)
            {
            case ASTExpression::Operator::ADD:
                result = integer1 + integer2;
                break;
            case ASTExpression::Operator::MINUS:
                result = integer1 - integer2;
                break;
            case ASTExpression::Operator::MULTIPLY:
                result = integer1 * integer2;
                break;
            case ASTExpression::Operator::DIVIDE:
                result = integer1 / integer2;
                break;
            }
            evaluationStack.push(SQLValue(result));
        }
        else if (term->type == term->INTEGER)
        {
            evaluationStack.push(SQLValue(term->intValue));
        }
        else if (term->type == term->STRING)
        {
            evaluationStack.push(SQLValue(term->stringValue));
        }
        else if (term->type == term->COLUMN_NAME)
        {
            evaluationStack.push(context[term->stringValue]);
        }
    }
    auto ret = evaluationStack.top();
    evaluationStack.pop();
    return ret;
}

void SuffixExpression::construct(const ASTExpression const * astExpression)
{
    if (astExpression->op == ASTExpression::Operator::NONE_CONSTANT)
    {
        auto valNode = astExpression->constant;
        if (valNode->dataType == ASTSQLDataValue::TYPE_NUMERICAL)
        {
            expression.push_back(ExpressionTerm::createIntegerValueTerm(
                std::stoi(valNode->value)));
        }
        else if (valNode->dataType == ASTSQLDataValue::TYPE_STRING)
        {
            expression.push_back(ExpressionTerm::createIntegerValueTerm(
                std::stoi(valNode->value)));
        }
        return;
    }
    else if(astExpression->op == ASTExpression::Operator::NONE_COLUMN_NAME)
    {
        expression.push_back(
            ExpressionTerm::createColumnNameTerm(astExpression->identifier->name)
        );
        requiredContex.insert(astExpression->identifier->name);
        return;
    }
    construct(astExpression->left);
    construct(astExpression->right);
    expression.push_back(ExpressionTerm::createColumnOperatorTerm(astExpression->op));
}
