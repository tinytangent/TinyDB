%skeleton "lalr1.cc"
%require  "3.0"
%debug
%defines
%define parser_class_name {SQLParser}
%define api.namespace {TinyDB::Generated}

%code requires{
    #include "Parser/ASTNodeBase.h"
    #include "Parser/ASTNodes.h"
    #include <string>
    class SQLLexer;
}

%parse-param { SQLLexer &scanner }
%parse-param { ASTNodeBase* &result }

%code{
    #include <iostream>
    #include <cstdlib>
    #include <fstream>
    #include "Parser/SQLLexer.h"
    #undef yylex
    #define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%token END 0 "end of file"
%token CHAR
%token WHITESPACE NEWLINE

%token CREATE DROP ALTER
%token DATABASE TABLE
%token SMALLINT INTEGER BIGINT
%token NOT
%token NULLTOKEN UNIQUE
%token <std::string> IDENTIFIER

%type<ASTIdentifierNode*> Identifier
%type<ASTCreateDatabaseStmtNode*> CreateDatabaseStatement
%type<ASTDropDatabaseStmtNode*> DropDatabaseStatement

%locations

%%

SQL : Statement END;

NOTNULL : NOT NULLTOKEN;

DataType : SMALLINT | INTEGER | BIGINT;

Identifier :
    IDENTIFIER
    {
        $$ = new ASTIdentifierNode($1);
    };

CreateDatabaseStatement :
    CREATE DATABASE Identifier
    {
        $$ = new ASTCreateDatabaseStmtNode($3);
    };

DropDatabaseStatement :
    DROP DATABASE Identifier
    {
        $$ = new ASTDropDatabaseStmtNode($3);
    };

CreateTableFieldConstraint :
    UNIQUE | NULLTOKEN | NOTNULL;

CreateTableField :
    IDENTIFIER DataType CreateTableFieldConstraint;

CreateTableFieldList :
    CreateTableField | CreateTableField CreateTableFieldList;

CreateTableStatement :
    CREATE TABLE IDENTIFIER '(' ')'

Statement :
    CreateDatabaseStatement |
    DropDatabaseStatement;

%%


void TinyDB::Generated::SQLParser::error(const location_type &l, const std::string &err_message)
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
