%{
/* C declarations & preamble code */

#include <stdio.h>
#include <stdlib.h>

extern char *yytext;
extern int   yylex();
extern int   yyerror( char * str);

%}

/* Declarations */

%token TOKEN_EOF
%token TOKEN_ERROR

/* Literals */ 
%token TOKEN_IDENTIFIER
%token TOKEN_STRING_LITERAL
%token TOKEN_INTEGER_LITERAL
%token TOKEN_HEXIDECIMAL_LITERAL
%token TOKEN_BINARY_LITERAL
%token TOKEN_DOUBLE_LITERAL
%token TOKEN_DOUBLE_SCIENTIFIC_LITERAL
%token TOKEN_CHAR_LITERAL

/* Keywords */
%token TOKEN_ARRAY
%token TOKEN_AUTO
%token TOKEN_BOOLEAN
%token TOKEN_CARRAY
%token TOKEN_CHAR
%token TOKEN_ELSE
%token TOKEN_FALSE
%token TOKEN_FLOAT
%token TOKEN_DOUBLE
%token TOKEN_FOR
%token TOKEN_FUNCTION
%token TOKEN_IF
%token TOKEN_INTEGER
%token TOKEN_PRINT
%token TOKEN_RETURN
%token TOKEN_STRING
%token TOKEN_TRUE
%token TOKEN_VOID
%token TOKEN_WHILE

/* Expression */
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_LBRACE
%token TOKEN_RBRACE
%token TOKEN_LBRACKET
%token TOKEN_RBRACKET
%token TOKEN_INCREMENT
%token TOKEN_DECREMENT 
%token TOKEN_ARRAY_LEN
%token TOKEN_LOGICAL_NOT
%token TOKEN_EXPONENTIATION
%token TOKEN_MULTIPLICATION
%token TOKEN_DIVISION
%token TOKEN_REMINDER
%token TOKEN_ADDITION
%token TOKEN_SUBTRACTION
%token TOKEN_LESS_THAN
%token TOKEN_LESS_THAN_OR_EQUAL
%token TOKEN_GREATER_THAN
%token TOKEN_GREATER_THAN_OR_EQUAL
%token TOKEN_COMPARISON_EQUAL
%token TOKEN_COMPARISON_NOT_EQUAL
%token TOKEN_LOGICAL_AND
%token TOKEN_LOGICAL_OR
%token TOKEN_ASSIGNMENT

/* Punctuation */
%token TOKEN_SEMICOLON
%token TOKEN_COLON
%token TOKEN_COMMA


%%
/* Grammar Rules */

type: TOKEN_ARRAY 
    |   TOKEN_AUTO
    | /* epsilon */
%%
/* C postamble code */

int yyerror( char *s ) {
    printf("parse error: %s\n",s);
    return 1;
}
