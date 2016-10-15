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

(?i:create) {
    return token::CREATE;
}

(?i:drop) {
    return token::DROP;
}

(?i:database) {
    return token::DATABASE;
}

[a-zA-Z]+ {
    yylval->build< std::string >( yytext );
    return( token::IDENTIFIER );
}

. {
    //TODO: maybe this should trigger an error.
    return( token::CHAR );
}
%%
