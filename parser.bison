%{
/* C declarations & preamble code */

#include <stdio.h>
#include <stdlib.h>

extern char *yytext;
extern int   yylex();
extern int   yyerror(const char *str);
extern int   yylineno;

%}

/* Declarations */
/* additional errors */
%locations 
%define parse.error verbose

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

program:    decl_list 
            ;

decl_list:  decl decl_list 
            | /* epsilon */
            ;

decl:       var_decl
            | func_decl
            ;

/* Variable Declarations */
var_decl:   TOKEN_IDENTIFIER TOKEN_COLON type TOKEN_SEMICOLON;
            | TOKEN_IDENTIFIER TOKEN_COLON type TOKEN_ASSIGNMENT expr TOKEN_SEMICOLON
            | TOKEN_IDENTIFIER TOKEN_COLON type TOKEN_ASSIGNMENT TOKEN_LBRACE braced_expr TOKEN_RBRACE TOKEN_SEMICOLON
            ;

braced_expr:  non_braced_expr
              | /* epsilon */
              ;

non_braced_expr:        TOKEN_LBRACE non_braced_expr TOKEN_RBRACE TOKEN_COMMA non_braced_expr
                        | TOKEN_LBRACE non_braced_expr TOKEN_RBRACE
                        | expr_list
                        ;

/* Function Declarations */
func_decl:  TOKEN_IDENTIFIER TOKEN_COLON TOKEN_FUNCTION return_type TOKEN_LPAREN param_list TOKEN_RPAREN TOKEN_SEMICOLON
            | TOKEN_IDENTIFIER TOKEN_COLON TOKEN_FUNCTION return_type TOKEN_LPAREN param_list TOKEN_RPAREN TOKEN_ASSIGNMENT block 
            ;


/* Parameters */
param_list:  non_empty_param_list
            | /* epsilon */
            ;

non_empty_param_list:   param_decl TOKEN_COMMA non_empty_param_list
                        | param_decl
                        ;

param_decl: TOKEN_IDENTIFIER TOKEN_COLON type
            ;
    

/* types */
type:       array_type
            | atomic_type
            | TOKEN_AUTO
            ; 

atomic_type: TOKEN_INTEGER
            | TOKEN_DOUBLE
            | TOKEN_FLOAT
            | TOKEN_BOOLEAN
            | TOKEN_CHAR
            | TOKEN_STRING
            ;

return_type:  TOKEN_VOID 
            | atomic_type
            | TOKEN_AUTO
            ;

array_type: TOKEN_ARRAY TOKEN_LBRACKET expr TOKEN_RBRACKET type 
            | TOKEN_CARRAY TOKEN_LBRACKET expr TOKEN_RBRACKET type
            | TOKEN_ARRAY TOKEN_LBRACKET TOKEN_RBRACKET type
            | TOKEN_CARRAY TOKEN_LBRACKET TOKEN_RBRACKET type
            ;

/* Statements  */
stmt_list:  stmt stmt_list
            | /* epsilon */
            ;

stmt:       unmatched_stmt
            | matched_stmt 
            ;

unmatched_stmt:     TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN stmt
                    | TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN matched_stmt TOKEN_ELSE unmatched_stmt
                    | TOKEN_FOR TOKEN_LPAREN for_init TOKEN_SEMICOLON for_cond TOKEN_SEMICOLON for_updt TOKEN_RPAREN unmatched_stmt
                    ;    

matched_stmt:       TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN matched_stmt TOKEN_ELSE matched_stmt
                    | TOKEN_FOR TOKEN_LPAREN for_init TOKEN_SEMICOLON for_cond TOKEN_SEMICOLON for_updt TOKEN_RPAREN matched_stmt
                    | base_stmt 
                    ;

for_init:   expr
            | /* epsilon */
            ;

for_cond:   expr
            | /* epsilon */
            ;

for_updt:   expr   
            | /* epsilon */
            ;

base_stmt:  var_decl
            | expr_stmt
            | print_stmt
            | return_stmt
            | block
            /* no while loop */
            ;

expr_stmt:  expr TOKEN_SEMICOLON
            ;

print_stmt: TOKEN_PRINT print_list TOKEN_SEMICOLON
            | TOKEN_PRINT TOKEN_SEMICOLON
            ;

print_list: expr TOKEN_COMMA print_list
            | expr
            ;

return_stmt: TOKEN_RETURN TOKEN_SEMICOLON
            | TOKEN_RETURN expr TOKEN_SEMICOLON
            ;

block: TOKEN_LBRACE stmt_list TOKEN_RBRACE
            ;

/* Expressions */
expr:           assign_expr
                ;

assign_expr:     or_expr 
                | or_expr TOKEN_ASSIGNMENT or_expr
                ;

or_expr:        and_expr 
                | or_expr TOKEN_LOGICAL_OR and_expr 
                ;

and_expr:       eq_expr
                | and_expr TOKEN_LOGICAL_AND eq_expr
                ;

eq_expr:        comp_expr
                | eq_expr TOKEN_COMPARISON_EQUAL comp_expr
                | eq_expr TOKEN_COMPARISON_NOT_EQUAL comp_expr
                ;

comp_expr:      add_expr
                | comp_expr TOKEN_LESS_THAN add_expr
                | comp_expr TOKEN_LESS_THAN_OR_EQUAL add_expr
                | comp_expr TOKEN_GREATER_THAN add_expr
                | comp_expr TOKEN_GREATER_THAN_OR_EQUAL add_expr
                ;

add_expr:       mul_expr
                | add_expr TOKEN_ADDITION mul_expr
                | add_expr TOKEN_SUBTRACTION mul_expr
                ;

mul_expr:       exp_expr
                | mul_expr TOKEN_MULTIPLICATION exp_expr
                | mul_expr TOKEN_DIVISION exp_expr
                | mul_expr TOKEN_REMINDER exp_expr
                ;

exp_expr:       unary_expr
                | unary_expr TOKEN_EXPONENTIATION exp_expr
                ;

unary_expr:     TOKEN_LOGICAL_NOT unary_expr 
                | TOKEN_SUBTRACTION  unary_expr
                | TOKEN_ARRAY_LEN unary_expr
                | postfix_expr
                ;


postfix_expr:    literals_expr  
                | postfix_expr TOKEN_LBRACKET expr TOKEN_RBRACKET 
                | postfix_expr TOKEN_LPAREN expr_list TOKEN_RPAREN 
                | postfix_expr TOKEN_INCREMENT
                | postfix_expr TOKEN_DECREMENT
                ;

literals_expr: TOKEN_STRING_LITERAL
                | TOKEN_INTEGER_LITERAL
                | TOKEN_HEXIDECIMAL_LITERAL
                | TOKEN_BINARY_LITERAL
                | TOKEN_DOUBLE_LITERAL
                | TOKEN_DOUBLE_SCIENTIFIC_LITERAL
                | TOKEN_CHAR_LITERAL
                | TOKEN_TRUE
                | TOKEN_FALSE
                | TOKEN_IDENTIFIER
                | TOKEN_LPAREN expr TOKEN_RPAREN
                ;

expr_list:     expr TOKEN_COMMA expr_list 
              | expr
              ;

%%
/* C postamble code */

int yyerror(const char *s ) {
    printf("parse error at %d:  %s\n", yylineno, s);
    return 1;
}
