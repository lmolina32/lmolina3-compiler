%{
/* C declarations & preamble code */

#include <stdio.h>
#include <stdlib.h>

#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"

extern char *yytext;
extern int   yylex();
extern int   yyerror(const char *str);
extern int   yylineno;

Decl *root = 0;

%}

/* Require the header to include types */
%code requires {
    #include "decl.h"
    #include "expr.h"
    #include "param_list.h"
    #include "stmt.h"
    #include "symbol.h"
    #include "type.h"
}

/* Declarations */
/* additional errors */
%locations 
%define parse.error verbose

%token TOKEN_ERROR

/* Literals */ 
%token <name> TOKEN_IDENTIFIER
%token <string> TOKEN_STRING_LITERAL
%token <int_literal>TOKEN_INTEGER_LITERAL
%token <int_literal>TOKEN_HEXIDECIMAL_LITERAL
%token <int_literal>TOKEN_BINARY_LITERAL
%token <double_lit>TOKEN_DOUBLE_LITERAL
%token <double_lit>TOKEN_DOUBLE_SCIENTIFIC_LITERAL
%token <string> TOKEN_CHAR_LITERAL

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

/* AST */
%union {
    Decl *decl;
    Stmt *stmt;
    Type *type;
    Param_list *param_list;
    Expr *expr;
    Symbol *symbol;
    char *name;
    char *string; 
    int *int_literal;
    double *double_lit; 
}

%type <decl> program decl_list decl var_decl func_decl
%type <stmt> stmt_list stmt unmatched_stmt matched_stmt base_stmt expr_stmt print_stmt return_stmt block
%type <expr> array_init array_init_list array_init_element for_init for_cond for_updt expr assign_expr or_expr and_expr eq_expr comp_expr add_expr mul_expr exp_expr unary_expr postfix_expr literals_expr expr_list non_empty_expr_list id expr_list_init
%type <param_list> param_list non_empty_param_list param_decl
%type <type> type atomic_type return_type array_type
 


%%
/* Grammar Rules */

program:    decl_list 
            { root = $1; }
            ;

decl_list:  decl decl_list 
              { $$ = $1; $1->next = $2; }
            | /* epsilon */
              { $$ = 0; }
            ;

decl:       var_decl
              { $$ = $1; }
            | func_decl
              { $$ = $1; }
            ;

/* Variable Declarations */
var_decl:   id TOKEN_COLON type TOKEN_SEMICOLON
                { $$ = decl_create($1->name, $3, 0, 0, 0); expr_destroy($1); }
            | id TOKEN_COLON type TOKEN_ASSIGNMENT expr TOKEN_SEMICOLON
                { $$ = decl_create($1->name, $3, $5, 0, 0); expr_destroy($1); }
            | id TOKEN_COLON type TOKEN_ASSIGNMENT array_init TOKEN_SEMICOLON
                { $$ = decl_create($1->name, $3, $5, 0, 0); expr_destroy($1); }
            ;

array_init:     TOKEN_LBRACE array_init_list TOKEN_RBRACE
                    { $$ = expr_create(EXPR_BRACES, 0, $2); }
                ;

array_init_list:    array_init_element TOKEN_COMMA array_init_list
                        { $$ = $1; $1->right = $3; }
                    | array_init_element
                        { $$ = $1; }
                    ;

array_init_element: literals_expr
                        { $$ = expr_create(EXPR_ARGS, $1, 0); }
                    | array_init
                        { $$ = expr_create(EXPR_ARGS, $1, 0); }
                    ;

/* Function Declarations */
func_decl:  id TOKEN_COLON TOKEN_FUNCTION return_type TOKEN_LPAREN param_list TOKEN_RPAREN TOKEN_SEMICOLON
                { $$ = decl_create($1->name, type_create(TYPE_FUNCTION, $4, $6, 0), 0, 0, 0); expr_destroy($1); }
            | id TOKEN_COLON TOKEN_FUNCTION return_type TOKEN_LPAREN param_list TOKEN_RPAREN TOKEN_ASSIGNMENT block 
                { $$ = decl_create($1->name, type_create(TYPE_FUNCTION, $4, $6, 0), 0, $9, 0); expr_destroy($1); }
            ;

/* identifiers */

id:     TOKEN_IDENTIFIER
            { $$ = expr_create_name($1); free($1); }
        ;

/* Parameters */
param_list:  non_empty_param_list
                { $$ = $1; }
            | /* epsilon */
                { $$ = 0; }
            ;

non_empty_param_list:   param_decl TOKEN_COMMA non_empty_param_list
                            { $$ = $1; $1->next = $3; }
                        | param_decl
                            { $$ = $1; }
                        ;

param_decl: id TOKEN_COLON type
                { $$ = param_list_create($1->name, $3, 0); expr_destroy($1); }
            ;
    

/* types */
type:       array_type
                { $$ = $1; }
            | atomic_type
                { $$ = $1; }
            | TOKEN_AUTO
                { $$ = type_create(TYPE_AUTO, 0, 0, 0); }
            ; 

atomic_type: TOKEN_INTEGER
                { $$ = type_create(TYPE_INTEGER, 0, 0, 0); }
            | TOKEN_DOUBLE
                { $$ = type_create(TYPE_DOUBLE, 0, 0, 0); }
            | TOKEN_FLOAT
                { $$ = type_create(TYPE_DOUBLE, 0, 0, 0); }
            | TOKEN_BOOLEAN
                { $$ = type_create(TYPE_BOOLEAN, 0, 0, 0); }
            | TOKEN_CHAR
                { $$ = type_create(TYPE_CHARACTER, 0, 0, 0); }
            | TOKEN_STRING
                { $$ = type_create(TYPE_STRING, 0, 0, 0); }
            ;

return_type:  TOKEN_VOID 
                { $$ = type_create(TYPE_VOID, 0, 0, 0); }
            | atomic_type
                { $$ = $1; }
            | TOKEN_AUTO
                { $$ = type_create(TYPE_AUTO, 0, 0, 0); }
            ;

array_type: TOKEN_ARRAY TOKEN_LBRACKET expr TOKEN_RBRACKET type 
                { $$ = type_create(TYPE_ARRAY, $5, 0, $3); }
            | TOKEN_CARRAY TOKEN_LBRACKET expr TOKEN_RBRACKET type
                { $$ = type_create(TYPE_CARRAY, $5, 0, $3); }
            | TOKEN_ARRAY TOKEN_LBRACKET TOKEN_RBRACKET type
                { $$ = type_create(TYPE_ARRAY, $4, 0, 0); }
            | TOKEN_CARRAY TOKEN_LBRACKET TOKEN_RBRACKET type
                { $$ = type_create(TYPE_CARRAY, $4, 0, 0); }
            ;

/* Statements  */
stmt_list:  stmt stmt_list
                { $$ = $1; $1->next = $2; }
            | /* epsilon */
                { $$ = 0;}
            ;

stmt:       unmatched_stmt
                { $$ = $1; }
            | matched_stmt 
                { $$ = $1; }
            ;

unmatched_stmt:     TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt
                        {$$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, 0, 0); }
                    | TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN matched_stmt TOKEN_ELSE unmatched_stmt
                        { $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, $7, 0); }
                    | TOKEN_FOR TOKEN_LPAREN for_init TOKEN_SEMICOLON for_cond TOKEN_SEMICOLON for_updt TOKEN_RPAREN unmatched_stmt
                        { $$ = stmt_create(STMT_FOR, 0, $3, $5, $7, $9, 0, 0); }
                    ;    

matched_stmt:       TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN matched_stmt TOKEN_ELSE matched_stmt
                        { $$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, $7, 0); }
                    | TOKEN_FOR TOKEN_LPAREN for_init TOKEN_SEMICOLON for_cond TOKEN_SEMICOLON for_updt TOKEN_RPAREN matched_stmt
                        { $$ = stmt_create(STMT_FOR, 0, $3, $5, $7, $9, 0, 0); }
                    | base_stmt 
                        { $$ = $1; }
                    ;

for_init:   expr
                { $$ = $1; }
            | /* epsilon */
                { $$ = 0; }
            ;

for_cond:   expr
                { $$ = $1; }
            | /* epsilon */
                { $$ = 0; }
            ;

for_updt:   expr   
                { $$ = $1; }
            | /* epsilon */
                { $$ = 0; }
            ;

base_stmt:  var_decl
                {$$ = stmt_create(STMT_DECL, $1, 0, 0, 0, 0, 0, 0); }
            | expr_stmt
                {$$ = $1; }
            | print_stmt
                {$$ = $1; }
            | return_stmt
                {$$ = $1; }
            | block
                {$$ = $1; }
            /* no while loop */
            ;

expr_stmt:  expr TOKEN_SEMICOLON
                {$$ = stmt_create(STMT_EXPR, 0, 0, $1, 0, 0, 0, 0); }
            ;

print_stmt: TOKEN_PRINT non_empty_expr_list TOKEN_SEMICOLON
                { $$ = stmt_create(STMT_PRINT, 0, 0, $2, 0, 0, 0, 0); }
            | TOKEN_PRINT TOKEN_SEMICOLON
                { $$ = stmt_create(STMT_PRINT, 0, 0, 0, 0, 0, 0, 0); }
            ;

return_stmt: TOKEN_RETURN TOKEN_SEMICOLON
                { $$ = stmt_create(STMT_RETURN, 0, 0, 0, 0, 0, 0, 0); }
            | TOKEN_RETURN expr TOKEN_SEMICOLON
                { $$ = stmt_create(STMT_RETURN, 0, 0, $2, 0, 0, 0, 0); }
            ;

block: TOKEN_LBRACE stmt_list TOKEN_RBRACE
                {$$ = stmt_create(STMT_BLOCK, 0, 0, 0, 0, $2, 0, 0);}
            ;

/* Expressions */
expr:           assign_expr
                    { $$ = $1; }
                ;

assign_expr:     or_expr 
                    { $$ = $1; }
                | or_expr TOKEN_ASSIGNMENT assign_expr 
                    { $$ = expr_create(EXPR_ASSIGN, $1, $3); }
                ;

or_expr:        and_expr 
                    { $$ = $1; }
                | or_expr TOKEN_LOGICAL_OR and_expr 
                    { $$ = expr_create(EXPR_OR, $1, $3); }
                ;

and_expr:       eq_expr
                    { $$ = $1; }
                | and_expr TOKEN_LOGICAL_AND eq_expr
                    { $$ = expr_create(EXPR_AND, $1, $3); }
                ;

eq_expr:        comp_expr
                    { $$ = $1; }
                | eq_expr TOKEN_COMPARISON_EQUAL comp_expr
                    { $$ = expr_create(EXPR_EQ, $1, $3); }
                | eq_expr TOKEN_COMPARISON_NOT_EQUAL comp_expr
                    { $$ = expr_create(EXPR_NOT_EQ, $1, $3); }
                ;

comp_expr:      add_expr
                    { $$ = $1; }
                | comp_expr TOKEN_LESS_THAN add_expr
                    { $$ = expr_create(EXPR_LT, $1, $3); }
                | comp_expr TOKEN_LESS_THAN_OR_EQUAL add_expr
                    { $$ = expr_create(EXPR_LTE, $1, $3); }
                | comp_expr TOKEN_GREATER_THAN add_expr
                    { $$ = expr_create(EXPR_GT, $1, $3); }
                | comp_expr TOKEN_GREATER_THAN_OR_EQUAL add_expr
                    { $$ = expr_create(EXPR_GTE, $1, $3); }
                ;

add_expr:       mul_expr
                    { $$ = $1; }
                | add_expr TOKEN_ADDITION mul_expr
                    { $$ = expr_create(EXPR_ADD, $1, $3); }
                | add_expr TOKEN_SUBTRACTION mul_expr
                    { $$ = expr_create(EXPR_SUB, $1, $3); }
                ;

mul_expr:       exp_expr
                    { $$ = $1; }
                | mul_expr TOKEN_MULTIPLICATION exp_expr
                    { $$ = expr_create(EXPR_MUL, $1, $3); }
                | mul_expr TOKEN_DIVISION exp_expr
                    { $$ = expr_create(EXPR_DIV, $1, $3); }
                | mul_expr TOKEN_REMINDER exp_expr
                    { $$ = expr_create(EXPR_REM, $1, $3); }
                ;

exp_expr:       unary_expr
                    { $$ = $1; }
                | unary_expr TOKEN_EXPONENTIATION exp_expr
                    { $$ = expr_create(EXPR_EXPO, $1, $3); }
                ;

unary_expr:     TOKEN_LOGICAL_NOT unary_expr 
                    { $$ = expr_create(EXPR_NOT, $2, 0); }
                | TOKEN_SUBTRACTION  unary_expr
                    { $$ = expr_create(EXPR_NEGATION, $2, 0); }
                | TOKEN_ARRAY_LEN unary_expr
                    { $$ = expr_create(EXPR_ARR_LEN, $2, 0); }
                | postfix_expr
                    { $$ = $1; }
                ;


postfix_expr:    literals_expr  
                    { $$ = $1; }
                | postfix_expr TOKEN_LBRACKET expr TOKEN_RBRACKET 
                    { $$ = expr_create(EXPR_INDEX, $1, $3);}
                | postfix_expr TOKEN_LPAREN expr_list TOKEN_RPAREN 
                    { $$ = expr_create(EXPR_FUNC, $1, $3);}
                | postfix_expr TOKEN_INCREMENT
                    { $$ = expr_create(EXPR_INCREMENT, $1, 0); }
                | postfix_expr TOKEN_DECREMENT
                    { $$ = expr_create(EXPR_DECREMENT, $1, 0); }
                ;

literals_expr: TOKEN_STRING_LITERAL
                    { $$ = expr_create_string_literal($1); free($1); }
                | TOKEN_INTEGER_LITERAL
                    { $$ = expr_create_integer_literal(*$1); free($1); }
                | TOKEN_HEXIDECIMAL_LITERAL
                    { $$ = expr_create_integer_literal(*$1); free($1); }
                | TOKEN_BINARY_LITERAL
                    { $$ = expr_create_integer_literal(*$1); free($1); }
                | TOKEN_DOUBLE_LITERAL
                    { $$ = expr_create_double_literal(*$1); free($1); }
                | TOKEN_DOUBLE_SCIENTIFIC_LITERAL
                    { $$ = expr_create_double_literal(*$1); free($1); }
                | TOKEN_CHAR_LITERAL
                    { $$ = expr_create_char_literal($1); free($1); }
                | TOKEN_TRUE
                    { $$ = expr_create_boolean_literal(1); }
                | TOKEN_FALSE
                    { $$ = expr_create_boolean_literal(0); }
                | TOKEN_IDENTIFIER
                    { $$ = expr_create_name($1); free($1); }
                | TOKEN_LPAREN expr TOKEN_RPAREN
                    { $$ = expr_create(EXPR_GROUPS, $2, 0); }
                ;

expr_list:     non_empty_expr_list
                    { $$ = $1; }
               | /* epsilon */
                    { $$ = 0; }
               ;

non_empty_expr_list:    expr_list_init TOKEN_COMMA non_empty_expr_list 
                            { $$ = $1; $1->right = $3; }
                        | expr_list_init
                            { $$ = $1; }
                        ;

expr_list_init:         expr
                            { $$ = expr_create(EXPR_ARGS, $1, 0); }
                        ;

%%
/* C postamble code */

int yyerror(const char *s ) {
    printf("parse error at %d:  %s\n", yylineno, s);
    return 1;
}
