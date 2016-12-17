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
%token OTHER_CHAR
%token WHITESPACE NEWLINE

%token CREATE DROP ALTER INSERT SELECT SHOW USE DELETE UPDATE
%token DATABASE DATABASES TABLE TABLES
%token SMALLINT INTEGER BIGINT
%token CHARACTER CHAR VARYING VARCHAR TEXT
%token NOT
%token NULLTOKEN CHECK DEFAULT UNIQUE REFERENCES
%token PRIMARY KEY
%token INTO FROM WHERE VALUES SET
%token <std::string> IDENTIFIER
%token <std::string> NUMERICAL STRING
%token ASTERISK
%token PLUS MINUS DIVIDE MOD
%token EQUAL NOT_EQUAL GREATER_THAN LESS_THAN
%token GREATER_THAN_OR_EQUAL LESS_THAN_OR_EQUAL

%nonassoc EQUAL NOT_EQUAL
%left PLUS MINUS
%left ASTERISK DIVIDE MOD

%type<ASTIdentifierNode*> Identifier
%type<std::vector<ASTIdentifierNode*>> IdentifierList
%type<ASTSQLDataType*> SQLDataType
%type<ASTCreateDatabaseStmtNode*> CreateDatabaseStatement
%type<ASTDropDatabaseStmtNode*> DropDatabaseStatement
%type<ASTShowDatabasesStmtNode*> ShowDatabasesStatement
%type<ASTUseDatabaseStmtNode*> UseDatabaseStatement
%type<ASTFieldConstraintNode*> FieldConstraint
%type<std::list<ASTFieldConstraintNode*>> FieldConstraintList
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
%type<ASTUpdateStmtNode*> UpdateStatement
%type<ASTDeleteStmtNode*> DeleteStatement
%type<ASTDropTableStmtNode*> DropTableStatement
%type<ASTShowTablesStmtNode*> ShowTablesStatement

%locations

%%

SQL :
    Statement END
    {
        result = $1;
    };

NOTNULL : NOT NULLTOKEN;
PRIMARYKEY : PRIMARY KEY;

VarCharType : VARCHAR | CHARACTER VARYING;
CharType : CHAR | CHARACTER;

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
    }
    | CharType '(' NUMERICAL ')'
    {
        $$ = new ASTSQLCharacterType(false, true, $3);
    }
    | VarCharType '(' NUMERICAL ')'
    {
        $$ = new ASTSQLCharacterType(false, false, $3);
    }
    | TEXT
    {
        $$ = new ASTSQLCharacterType(true, false, 0);
    }

SQLDataValue :
    NUMERICAL
    {
        $$ = new ASTSQLDataValue(ASTSQLDataValue::TYPE_NUMERICAL, $1);
    }
    | STRING
    {
        $$ = new ASTSQLDataValue(ASTSQLDataValue::TYPE_STRING, $1);
    };

Identifier :
    IDENTIFIER
    {
        $$ = new ASTIdentifierNode($1);
    };

IdentifierList :
    Identifier
    {
        $$ = std::vector<ASTIdentifierNode*>();
        $$.push_back($1);
    }
    | IdentifierList ',' Identifier
    {
        $$ = $1;
        $$.push_back($3);
    }

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
    }
    | Expression PLUS Expression
    {
        $$ = new ASTExpression(ASTExpression::Operator::ADD, $1, $3);
    }
    | Expression MINUS Expression
    {
        $$ = new ASTExpression(ASTExpression::Operator::MINUS, $1, $3);
    }
    | Expression ASTERISK Expression
    {
        $$ = new ASTExpression(ASTExpression::Operator::MULTIPLY, $1, $3);
    }
    | Expression DIVIDE Expression
    {
        $$ = new ASTExpression(ASTExpression::Operator::DIVIDE, $1, $3);
    }
    | '(' Expression ')'
    {
        $$ = $2;
    }

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

ShowDatabasesStatement :
    SHOW DATABASES
    {
        $$ = new ASTShowDatabasesStmtNode();
    }

FieldConstraint :
    NOTNULL
    {
        $$ = new ASTFieldConstraintNode(ASTFieldConstraintNode::Type::CONSTRAINT_NOT_NULL);
    }
    | NULLTOKEN
    {
        $$ = new ASTFieldConstraintNode(ASTFieldConstraintNode::Type::CONSTRAINT_NULL);
    }
    | CHECK '(' Expression ')'
    {
        $$ = new ASTFieldConstraintNode(
            ASTFieldConstraintNode::Type::CONSTRAINT_NONE, $3, "", "");
    }
    | DEFAULT Expression
    {
        $$ = new ASTFieldConstraintNode(
            ASTFieldConstraintNode::Type::CONSTRAINT_NONE, $2, "", "");
    }
    | UNIQUE
    {
        $$ = new ASTFieldConstraintNode(
            ASTFieldConstraintNode::Type::CONSTRAINT_UNIQUE);
    }
    | PRIMARYKEY
    {
        $$ = new ASTFieldConstraintNode(
            ASTFieldConstraintNode::Type::CONSTRAINT_PRIMARY_KEY);
    }
    | REFERENCES IDENTIFIER
    {
        $$ = new ASTFieldConstraintNode(
            ASTFieldConstraintNode::Type::CONSTRAINT_REFERENCES, nullptr, $2, "");
    }
    | REFERENCES IDENTIFIER '(' IDENTIFIER ')'
    {
        $$ = new ASTFieldConstraintNode(
            ASTFieldConstraintNode::Type::CONSTRAINT_REFERENCES, nullptr, $2, $4);
    }

FieldConstraintList :
    %empty
    {
        $$ = std::list<ASTFieldConstraintNode*>();
    }
    | FieldConstraintList FieldConstraint
    {
        $$ = $1;
        $$.push_back($2);
    };

CreateTableField :
    IDENTIFIER SQLDataType FieldConstraintList
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

DropTableStatement :
    DROP TABLE IDENTIFIER
    {
        $$ = new ASTDropTableStmtNode($3);
    }

ShowTablesStatement :
    SHOW TABLES
    {
        $$ = new ASTShowTablesStmtNode();
    }

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

UpdateStatement :
    UPDATE IDENTIFIER SET IDENTIFIER EQUAL Expression WHERE Expression
    {
        $$ = new ASTUpdateStmtNode($2, $4, $6, $8);
    };

DeleteStatement :
    DELETE FROM IDENTIFIER WHERE Expression
    {
        $$ = new ASTDeleteStmtNode($3, $5);
    };

UseDatabaseStatement :
    USE DATABASE Identifier
    {
        $$ = new ASTUseDatabaseStmtNode($3);
    }

Statement :
    CreateDatabaseStatement     { $$ = $1; }
    | DropDatabaseStatement     { $$ = $1; }
    | ShowDatabasesStatement    { $$ = $1; }
    | UseDatabaseStatement      { $$ = $1; }
    | CreateTableStatement      { $$ = $1; }
    | InsertIntoStatement       { $$ = $1; }
    | SelectStatement           { $$ = $1; }
    | UpdateStatement           { $$ = $1; }
    | DeleteStatement           { $$ = $1; }
    | DropTableStatement        { $$ = $1; }
    | ShowTablesStatement       { $$ = $1; }

%%


void TinyDB::Generated::SQLParser::error(const location_type &l, const std::string &err_message)
{
    std::cerr << "Error: " << err_message << " at " << l << "\n";
}
