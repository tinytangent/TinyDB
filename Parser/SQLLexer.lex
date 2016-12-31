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

NEWLINE             (\r|\n|\r\n)
WHITESPACE          ([ \t]+)

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
(?i:select) { return token::SELECT; }
(?i:show) { return token::SHOW; }
(?i:use) { return token::USE; }
(?i:delete) { return token::DELETE; }
(?i:update) { return token::UPDATE; }

(?i:database) { return token::DATABASE; }
(?i:databases) { return token::DATABASES; }
(?i:table) { return token::TABLE; }
(?i:tables) { return token::TABLES; }
(?i:index) { return token::INDEX; }

(?i:smallint) { return token::SMALLINT; }
(?i:integer) { return token::INTEGER; }
(?i:int) { return token::INTEGER; }
(?i:bigint) { return token::BIGINT; }

(?i:not) { return token::NOT; }
(?i:null) { return token::NULLTOKEN; }
(?i:check) { return token::CHECK; }
(?i:default) { return token::DEFAULT; }
(?i:unique) { return token::UNIQUE; }
(?i:primary) { return token::PRIMARY; }
(?i:foreign) { return token::FOREIGN; }
(?i:key) { return token::KEY; }
(?i:references) { return token::REFERENCES; }

(?i:as) { return token::AS; }
(?i:on) { return token::ON; }
(?i:into) { return token::INTO; }
(?i:from) { return token::FROM; }
(?i:where) { return token::WHERE; }
(?i:set) { return token::SET; }
(?i:values) { return token::VALUES; }

(?i:character) { return token::CHARACTER; }
(?i:char) { return token::CHAR; }
(?i:varying) { return token::VARYING; }
(?i:varchar) { return token::VARCHAR; }
(?i:text) { return token::TEXT; }

"*" { return token::ASTERISK; }
"+" { return token::PLUS; }
"-" { return token::MINUS; }
"/" { return token::DIVIDE; }
"%" { return token::MOD; }

"=" { return token::EQUAL; }
("!="|"<>") { return token::NOT_EQUAL; }
">" { return token::GREATER_THAN; }
"<" { return token::LESS_THAN; }
">=" { return token::GREATER_THAN_OR_EQUAL; }
"<=" { return token::LESS_THAN_OR_EQUAL; }
(?i:and) { return token::AND; }
(?i:or) { return token::OR; }
(?i:not) { return token::NOT; }

([a-zA-Z][0-9a-zA-Z_]*) {
    yylval->build<std::string>(yytext);
    return token::IDENTIFIER;
}

. {
    //TODO: maybe this should trigger an error.
    //return( token::OTHER_CHAR );
    return yytext[0];
}
%%
