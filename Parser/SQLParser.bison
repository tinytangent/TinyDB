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
    #include <list>
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

%token CREATE DROP ALTER INSERT SELECT
%token DATABASE TABLE
%token SMALLINT INTEGER BIGINT
%token NOT
%token NULLTOKEN UNIQUE
%token INTO FROM WHERE VALUES
%token <std::string> IDENTIFIER
%token <std::string> NUMERICAL STRING
%token ASTERISK
%token PLUS MINUS DIVIDE MOD
%token EQUAL NOT_EQUAL GREATER_THAN LESS_THAN
%token GREATER_THAN_OR_EQUAL LESS_THAN_OR_EQUAL

%nonassoc EQUAL NOT_EQUAL

%type<ASTIdentifierNode*> Identifier
%type<ASTSQLDataType*> SQLDataType
%type<ASTCreateDatabaseStmtNode*> CreateDatabaseStatement
%type<ASTDropDatabaseStmtNode*> DropDatabaseStatement
%type<ASTCreateTableFieldConstraint> CreateTableFieldConstraint
%type<ASTCreateTableFieldNode*> CreateTableField
%type<std::list<ASTCreateTableFieldNode*>> CreateTableFieldList
%type<ASTCreateTableStmtNode*> CreateTableStatement
%type<ASTNodeBase*> Statement
%type<ASTNodeBase*> SQL
%type<ASTSQLDataValue*> SQLDataValue
%type<std::list<ASTSQLDataValue*>> InsertIntoValueList
%type<ASTInsertIntoStmtNode*> InsertIntoStatement
%type<ASTExpression*> Expression
%type<ASTSelectStmtNode*> SelectStatement

%locations

%%

SQL :
    Statement END
    {
        result = $1;
    };

NOTNULL : NOT NULLTOKEN;

SQLDataType :
    SMALLINT
    {
        $$ = new ASTSQLSmallIntDataType();
    }
    | INTEGER
    {
        $$ = new ASTSQLIntegerDataType();
    }
    | BIGINT
    {
        $$ = new ASTSQLBigIntDataType();
    };

SQLDataValue :
    NUMERICAL
    {
        $$ = new ASTSQLDataValue(ASTSQLDataValue::TYPE_NUMERICAL, $1);
    }
    | STRING
    {
        $$ = new ASTSQLDataValue(ASTSQLDataValue::TYPE_NUMERICAL, $1);
    };

Identifier :
    IDENTIFIER
    {
        $$ = new ASTIdentifierNode($1);
    };

Expression :
    Identifier
    {
        $$ = new ASTExpression($1);
    }
    | SQLDataValue
    {
        $$ = new ASTExpression($1);
    }
    | Expression EQUAL Expression
    {
        $$ = new ASTExpression(ASTExpression::Operator::EQUAL, $1, $3);
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
    %empty
    {
        $$ = CONSTRAINT_NONE;
    }
    | UNIQUE
    {
        $$ = CONSTRAINT_UNIQUE;
    }
    | NULLTOKEN
    {
        $$ = CONSTRAINT_NULL;
    }
    | NOTNULL
    {
        $$ = CONSTRAINT_NOT_NULL;
    };

CreateTableField :
    IDENTIFIER SQLDataType CreateTableFieldConstraint
    {
        $$ = new ASTCreateTableFieldNode($1, $2, $3);
    };

CreateTableFieldList :
    CreateTableField
    {
        $$ = std::list<ASTCreateTableFieldNode*>();
        $$.push_back($1);
    }
    | CreateTableFieldList ',' CreateTableField
    {
        $$ = $1;
        $$.push_back($3);
    };

CreateTableStatement :
    CREATE TABLE IDENTIFIER '(' CreateTableFieldList ')'
    {
        $$ = new ASTCreateTableStmtNode($3, $5);
    };

InsertIntoValueList :
    SQLDataValue
    {
        $$ = std::list<ASTSQLDataValue*>();
        $$.push_back($1);
    }
    | InsertIntoValueList ',' SQLDataValue
    {
        $$ = $1;
        $$.push_back($3);
    };

InsertIntoStatement :
    INSERT INTO IDENTIFIER VALUES '(' InsertIntoValueList ')'
    {
        $$ = new ASTInsertIntoStmtNode($3, $6);
    };

SelectStatement :
    SELECT ASTERISK FROM IDENTIFIER WHERE Expression
    {
        $$ = new ASTSelectStmtNode($4, $6);
    };

Statement :
    CreateDatabaseStatement { $$ = $1; }
    | DropDatabaseStatement { $$ = $1; }
    | CreateTableStatement  { $$ = $1; }
    | InsertIntoStatement   { $$ = $1; }
    | SelectStatement       { $$ = $1; };

%%


void TinyDB::Generated::SQLParser::error(const location_type &l, const std::string &err_message)
{
    std::cerr << "Error: " << err_message << " at " << l << "\n";
}
