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
			SQLValue result;
			switch (term->op)
			{
			case ASTExpression::Operator::ADD:
			{
				if (val1.type != SQLValue::INTEGER || val2.type != SQLValue::INTEGER)
				{
					result = SQLValue::errorValue();
					break;
				}
				auto integer1 = val1.integerValue;
				auto integer2 = val2.integerValue;
				result = integer1 + integer2;
			}
			break;
			case ASTExpression::Operator::MINUS:
			{
				if (val1.type != SQLValue::INTEGER || val2.type != SQLValue::INTEGER)
				{
					result = SQLValue::errorValue();
					break;
				}
				auto integer1 = val1.integerValue;
				auto integer2 = val2.integerValue;
				result = integer1 - integer2;
			}
			break;
			case ASTExpression::Operator::MULTIPLY:
			{
				if (val1.type != SQLValue::INTEGER || val2.type != SQLValue::INTEGER)
				{
					result = SQLValue::errorValue();
					break;
				}
				auto integer1 = val1.integerValue;
				auto integer2 = val2.integerValue;
				result = integer1 * integer2;
			}
			break;
			case ASTExpression::Operator::DIVIDE:
			{
				if (val1.type != SQLValue::INTEGER || val2.type != SQLValue::INTEGER)
				{
					result = SQLValue::errorValue();
					break;
				}
				auto integer1 = val1.integerValue;
				auto integer2 = val2.integerValue;
				result = integer1 / integer2;
				break;
			}
			case ASTExpression::Operator::MOD:
			{
				if (val1.type != SQLValue::INTEGER || val2.type != SQLValue::INTEGER)
				{
					result = SQLValue::errorValue();
					break;
				}
				auto integer1 = val1.integerValue;
				auto integer2 = val2.integerValue;
				result = integer1 % integer2;
				break;
			}
			case ASTExpression::Operator::EQUAL:
			{
				if (val1.type != val2.type)
				{
					result = SQLValue::errorValue();
					break;
				}
				if (val1.type == SQLValue::INTEGER)
				{
					result = SQLValue(val1.integerValue == val2.integerValue);
					break;
				}
				else if (val1.type == SQLValue::BOOLEAN)
				{
					result = SQLValue(val1.boolValue == val2.boolValue);
					break;
				}
				else if (val1.type == SQLValue::STRING)
				{
					result = SQLValue(val1.stringValue == val2.stringValue);
					break;
				}
				break;
			}
			case ASTExpression::Operator::LESS_THAN:
			{
				if (val1.type != val2.type)
				{
					result = SQLValue::errorValue();
					break;
				}
				if (val1.type == SQLValue::INTEGER)
				{
					result = SQLValue(val1.integerValue < val2.integerValue);
					break;
				}
				else if (val1.type == SQLValue::BOOLEAN)
				{
					result = SQLValue(val1.boolValue < val2.boolValue);
					break;
				}
				else if (val1.type == SQLValue::STRING)
				{
					result = SQLValue(val1.stringValue < val2.stringValue);
					break;
				}
				break;
			}
			case ASTExpression::Operator::LESS_THAN_OR_EQUAL:
			{
				if (val1.type != val2.type)
				{
					result = SQLValue::errorValue();
					break;
				}
				if (val1.type == SQLValue::INTEGER)
				{
					result = SQLValue(val1.integerValue <= val2.integerValue);
					break;
				}
				else if (val1.type == SQLValue::BOOLEAN)
				{
					result = SQLValue(val1.boolValue <= val2.boolValue);
					break;
				}
				else if (val1.type == SQLValue::STRING)
				{
					result = SQLValue(val1.stringValue <= val2.stringValue);
					break;
				}
				break;
			}
			case ASTExpression::Operator::GREATER_THAN:
			{
				if (val1.type != val2.type)
				{
					result = SQLValue::errorValue();
					break;
				}
				if (val1.type == SQLValue::INTEGER)
				{
					result = SQLValue(val1.integerValue > val2.integerValue);
					break;
				}
				else if (val1.type == SQLValue::BOOLEAN)
				{
					result = SQLValue(val1.boolValue > val2.boolValue);
					break;
				}
				else if (val1.type == SQLValue::STRING)
				{
					result = SQLValue(val1.stringValue > val2.stringValue);
					break;
				}
				break;
			}
			case ASTExpression::Operator::GREATER_THAN_OR_EQUAL:
			{
				if (val1.type != val2.type)
				{
					result = SQLValue::errorValue();
					break;
				}
				if (val1.type == SQLValue::INTEGER)
				{
					result = SQLValue(val1.integerValue >= val2.integerValue);
					break;
				}
				else if (val1.type == SQLValue::BOOLEAN)
				{
					result = SQLValue(val1.boolValue >= val2.boolValue);
					break;
				}
				else if (val1.type == SQLValue::STRING)
				{
					result = SQLValue(val1.stringValue >= val2.stringValue);
					break;
				}
				break;
			}
			case ASTExpression::Operator::OR:
			{
				if (val1.type != SQLValue::BOOLEAN || val2.type != SQLValue::BOOLEAN)
				{
					result = SQLValue::errorValue();
					break;
				}
				result = SQLValue(val1.boolValue || val2.boolValue);
				break;
			}
			case ASTExpression::Operator::AND:
			{
				if (val1.type != SQLValue::BOOLEAN || val2.type != SQLValue::BOOLEAN)
				{
					result = SQLValue::errorValue();
					break;
				}
				result = SQLValue(val1.boolValue && val2.boolValue);
				break;
			}
			case ASTExpression::Operator::NOT:
			{
				if (val1.type != SQLValue::BOOLEAN || val2.type != SQLValue::BOOLEAN)
				{
					result = SQLValue::errorValue();
					break;
				}
				result = SQLValue(val1.boolValue && val2.boolValue);
				break;
			}
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
        else if (term->type == term->BOOLEAN)
        {
            evaluationStack.push(SQLValue(term->boolValue));
        }
    }
    auto ret = evaluationStack.top();
    evaluationStack.pop();
    return ret;
}

void SuffixExpression::construct(const ASTExpression const * astExpression)
{
    if (!astExpression)
    {
        expression.push_back(
            ExpressionTerm::createBooleanValueTerm(true)
        );
        return;
    }
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
            expression.push_back(ExpressionTerm::createStringValueTerm(valNode->value));
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
	else if (astExpression->op == ASTExpression::Operator::NOT)
	{
		construct(astExpression->left);
		return;
	}
    construct(astExpression->left);
    construct(astExpression->right);
    expression.push_back(ExpressionTerm::createColumnOperatorTerm(astExpression->op));
}
