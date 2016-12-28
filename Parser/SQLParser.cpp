#include <sstream>
#include "SQLLexer.h"
#include "SQLParser.h"

std::vector<ASTNodeBase*> SQLParser::parse(const std::string& sql)
{
    std::vector<ASTNodeBase*> ret;
    std::istringstream stringStream(sql);
    SQLLexer lexer(&stringStream);
    TinyDB::Generated::SQLParser parser(lexer, ret);
    if(parser.parse() != 0)
    {
       return ret;
    }
    return ret;
}

std::vector<ASTNodeBase*> SQLParser::parseStream(std::istream &inputStream)
{
    std::vector<ASTNodeBase*> ret;
    SQLLexer lexer(&inputStream);
    TinyDB::Generated::SQLParser parser(lexer, ret);
    if (parser.parse() != 0)
    {
        return ret;
    }
    return ret;
}
