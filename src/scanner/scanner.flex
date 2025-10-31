%{
#include "utils.h"
#include "encoder.h"
#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
%}

/* Definitions */

WHITE_SPACE     [ \t\r\n]
C_STYLE         \/\*([^\*]|\*+[^\*\/])*\*+\/ 
C_PLUS_STYLE    \/\/.*
COMMENTS        ({C_STYLE}|{C_PLUS_STYLE}) 
NOT_COMMENTS    \/\*.*
IDENTIFIER      [a-zA-Z_][a-zA-Z_0-9]{0,254}
NOT_IDENT       [a-zA-Z_][a-zA-Z_0-9]{255,}

/* literals */
INTEGER         [0-9]+
HEXIDECIMAL     0x[0-9a-fA-F]+ 
BINARY          0b[01]+
SCIENTIFIC      ([0-9]+\.?[0-9]*[eE][+-]?[0-9]+(\.[0-9]+)?)
DOUBLE_VALUE    ([0-9]+\.[0-9]+)
BOOL            (true|false)
CHAR_BACKSLASH  (\\0x[0-9a-fA-F]{2}|\\[\x20-\x7f])
CHAR_VALUE      \'([\x00-\x26\x28-\xff]|{CHAR_BACKSLASH})\'
STRING_VALUE    \"([^\"\\\n]|\\.)*\" 

%option yylineno 

/* Rules */
%%

{WHITE_SPACE}+  { } 
{COMMENTS}      { }

array           { return TOKEN_ARRAY; }
auto            { return TOKEN_AUTO; }
boolean         { return TOKEN_BOOLEAN; }
carray          { return TOKEN_CARRAY; }
char            { return TOKEN_CHAR; }
else            { return TOKEN_ELSE; }
false           { return TOKEN_FALSE; }
float           { return TOKEN_FLOAT; }
double          { return TOKEN_DOUBLE; }
for             { return TOKEN_FOR; }
function        { return TOKEN_FUNCTION; }
if              { return TOKEN_IF; }
integer         { return TOKEN_INTEGER; }
print           { return TOKEN_PRINT; }
return          { return TOKEN_RETURN; }
string          { return TOKEN_STRING; }
true            { return TOKEN_TRUE; }
void            { return TOKEN_VOID; }
while           { return TOKEN_WHILE; }

"("             { return TOKEN_LPAREN; }
")"             { return TOKEN_RPAREN; }
"{"             { return TOKEN_LBRACE; }
"}"             { return TOKEN_RBRACE; }
"["             { return TOKEN_LBRACKET; }
"]"             { return TOKEN_RBRACKET; }
"++"            { return TOKEN_INCREMENT; }
"--"            { return TOKEN_DECREMENT; } 
"#"             { return TOKEN_ARRAY_LEN; }
"!"             { return TOKEN_LOGICAL_NOT; }
"^"             { return TOKEN_EXPONENTIATION; }
"*"             { return TOKEN_MULTIPLICATION; }
"/"             { return TOKEN_DIVISION; }
"%"             { return TOKEN_REMINDER; }
"+"             { return TOKEN_ADDITION; }
"-"             { return TOKEN_SUBTRACTION; }
"<"             { return TOKEN_LESS_THAN; }
"<="            { return TOKEN_LESS_THAN_OR_EQUAL; }
">"             { return TOKEN_GREATER_THAN; }
">="            { return TOKEN_GREATER_THAN_OR_EQUAL; }
"=="            { return TOKEN_COMPARISON_EQUAL; }
"!="            { return TOKEN_COMPARISON_NOT_EQUAL; }
"&&"            { return TOKEN_LOGICAL_AND; }
"||"            { return TOKEN_LOGICAL_OR; }
"="             { return TOKEN_ASSIGNMENT; }
";"             { return TOKEN_SEMICOLON; }
":"             { return TOKEN_COLON; }
","             { return TOKEN_COMMA; }

{STRING_VALUE}  {   // decode string lit and save in yylval 
                    char buffer[BUFSIZ];
                    if(!string_decode(yytext, buffer)){
                        return TOKEN_ERROR;
                    }
                    char *string_lit = safe_calloc(sizeof(char), strlen(buffer) + 1); 
                    strcpy(string_lit, buffer);
                    yylval.string = string_lit;

                    return TOKEN_STRING_LITERAL;
                }
{CHAR_VALUE}    {   // decode char literal and save in yylval
                    int len = strlen(yytext);
                    char buffer[BUFSIZ];

                    yytext[0] = '"';
                    yytext[len]  = '\0';
                    yytext[len - 1] = '"';


                    if(!string_decode(yytext, buffer)){
                        return TOKEN_ERROR;
                    }

                    char *char_lit = safe_calloc(sizeof(char), strlen(buffer) + 1); 
                    strcpy(char_lit, buffer);
                    yylval.string = char_lit;

                    return TOKEN_CHAR_LITERAL; 
                }

{BINARY}        {   // convert binary to integer and save in yylval 
                    char *binary_str = safe_calloc(sizeof(char), strlen(yytext) - 1); 
                    strncpy(binary_str, yytext + 2, strlen(yytext) - 2);
                    int *bin = safe_calloc(sizeof(int), 1);
                    errno = 0; 
                    char *endptr;
                    *bin = strtol(binary_str, &endptr, 2);
                    if (errno == ERANGE){
                        printf("Error: Overflow/Underflow for '%s'\n", binary_str);
                        free(binary_str);
                        free(bin);
                        exit(1);
                    }

                    free(binary_str);
                    yylval.int_literal = bin;
                    return TOKEN_BINARY_LITERAL; 
                }
{HEXIDECIMAL}   {   // convert hex to int and save in yylval 
                    int *hex = safe_calloc(sizeof(int), 1);
                    errno = 0;
                    char *endptr;
                    *hex = strtol(yytext, &endptr, 0);
                    if (errno == ERANGE){
                        printf("Error: Overflow/Underflow for '%s'\n", yytext);
                        free(hex);
                        exit(1);
                    }
                    yylval.int_literal = hex;
                    return TOKEN_HEXIDECIMAL_LITERAL; 
                }
{INTEGER}       {   // convert string int to int and save in yylval
                    int *integer = safe_calloc(sizeof(int), 1);
                    char *endptr;
                    errno = 0;
                    *integer = strtol(yytext, &endptr, 10);
                    if (errno == ERANGE){
                        printf("Error: Overflow/Underflow for '%s'\n", yytext);
                        free(integer);
                        exit(1);
                    }
                    yylval.int_literal = integer;
                    return TOKEN_INTEGER_LITERAL;
                }
{SCIENTIFIC}    {   // convert string sci to int and save in yylval
                    double *sci = safe_calloc(sizeof(double), 1);
                    char *endptr;
                    errno = 0;
                    *sci = strtod(yytext, &endptr);
                    if (errno == ERANGE){
                        printf("Error: Overflow/Underflow for '%s'\n", yytext);
                        free(sci);
                        exit(1);
                    }
                    yylval.double_lit = sci;
                    return TOKEN_DOUBLE_SCIENTIFIC_LITERAL; 
                }
{DOUBLE_VALUE}  {   // convert string double to double and save in yylval
                    double *d = safe_calloc(sizeof(double), 1);
                    char *endptr;
                    errno = 0;
                    *d = strtod(yytext, &endptr);
                    if (errno == ERANGE){
                        printf("Error: Overflow/Underflow for '%s'\n", yytext);
                        free(d);
                        exit(1);
                    }
                    yylval.double_lit = d;
                    return TOKEN_DOUBLE_LITERAL; 
                }

{IDENTIFIER}    {   // save identifier in name  
                    yylval.name = safe_strdup(yytext); 
                    return TOKEN_IDENTIFIER; 
                }
{NOT_IDENT}     { return TOKEN_ERROR; }

.               { return TOKEN_ERROR; }

%%
/* User Code */
int yywrap() { return 1; }