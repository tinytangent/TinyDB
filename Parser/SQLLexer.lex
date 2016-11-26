%{
#include <string>

/* Implementation of yyFlexScanner */
#include "Parser/SQLLexer.h"
#undef  YY_DECL
#define YY_DECL int SQLLexer::yylex( \
    TinyDB::Generated::SQLParser::semantic_type *const lval, \
    TinyDB::Generated::SQLParser::location_type *loc)

/* typedef to make the returns for the tokens shorter */
using token = TinyDB::Generated::SQLParser::token;

/* define yyterminate as this instead of NULL */
#define yyterminate() return( token::END )

/* msvc2010 requires that we exclude this header file. */
#define YY_NO_UNISTD_H

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option debug
%option nodefault
%option yyclass="SQLLexer"
%option noyywrap
%option c++

NEWLINE         (\r|\n|\r\n)
WHITESPACE			([ \t]+)

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

{WHITESPACE} {
    //Skip whitespace
    ;
}

{NEWLINE} {
    //Skip new lines
    ;
}

\'(\\.|[^'])*\' {
    std::string text = yytext;
    text = text.substr(1, text.size() - 2);
    yylval->build<std::string>(text);
    return token::STRING;
}

([+-]?[0-9]+) {
    yylval->build<std::string>(yytext);
    return token::NUMERICAL;
}

(?i:create) { return token::CREATE; }
(?i:drop) { return token::DROP; }
(?i:alter) { return token::ALTER; }
(?i:insert) { return token::INSERT; }

(?i:database) { return token::DATABASE; }
(?i:table) { return token::TABLE; }

(?i:smallint) { return token::SMALLINT; }
(?i:integer) { return token::INTEGER; }
(?i:int) { return token::INTEGER; }
(?i:bigint) { return token::BIGINT; }

(?i:not) { return token::NOT; }
(?i:null) { return token::NULLTOKEN; }
(?i:unique) { return token::UNIQUE; }

(?i:into) { return token::INTO; }
(?i:values) { return token::VALUES; }

[a-zA-Z]+ {
    yylval->build<std::string>(yytext);
    return token::IDENTIFIER;
}

. {
    //TODO: maybe this should trigger an error.
    //return( token::CHAR );
    return yytext[0];
}
%%
