#ifndef __TINYDB_SQL_LEXER_H__
#define __TINYDB_SQL_LEXER_H__

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "SQLParser.tab.hpp"

class SQLLexer : public yyFlexLexer{
public:
    TinyDB::Generated::SQLParser* parser;
    SQLLexer(std::istream *in) : yyFlexLexer(in)
    {
        loc = new TinyDB::Generated::SQLParser::location_type();
    };
    virtual ~SQLLexer() {
        delete loc;
    };
    virtual int yylex(
        TinyDB::Generated::SQLParser::semantic_type * const lval,
        TinyDB::Generated::SQLParser::location_type *location );
   // YY_DECL defined in mc_lexer.l
   // Method body created by flex in mc_lexer.yy.cc

protected:
    TinyDB::Generated::SQLParser::semantic_type *yylval = nullptr;
    TinyDB::Generated::SQLParser::location_type *loc    = nullptr;
};

#endif /* END __MCSCANNER_HPP__ */
