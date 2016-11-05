#include <sstream>
#include "SQLLexer.h"
#include "SQLParser.h"

ASTNodeBase* SQLParser::parse(const std::string& sql)
{
    ASTNodeBase *ret;
    std::istringstream stringStream(sql);
    SQLLexer lexer(&stringStream);
    TinyDB::Generated::SQLParser parser(lexer, ret);
    if(parser.parse() != 0)
    {
       return NULL;
    }
    return ret;
}
