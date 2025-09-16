%{
#include "tokens.h"
%}

/* Definitions */

WHITE_SPACE     [ \t\r\n]
C_STYLE         \/\*([^\*]|\*+[^\*\/])*\*+\/
C_PLUS_STYLE    \/\/.*
COMMENTS        ({C_STYLE}|{C_PLUS_STYLE}) 
NOT_COMMENTS    \/\*.*
IDENTIFIER      [a-zA-Z_][a-zA-Z_0-9]{0,254}

INTEGER         [0-9]+
HEXIDECIMAL     [0-9a-fA-F]+H 
BINARY          [01]+B

SCIENTIFIC      ([0-9]+\.?[0-9]*[eE][+-]?[0-9]+(\.[0-9]+)*)
DOUBLE_VALUE    ([0-9]+\.[0-9]+)

BOOL            (true|false)

CHAR_BACKSLASH  (\\0x[0-9a-fA-F]{2}|\\[\x20-\x7e])
CHAR_VALUE      \'([\x00-\xff]|{CHAR_BACKSLASH})\'

STRING_VALUE    \"([^\"\\\n]|\\.)*\" 

/* Rules */
%%

{WHITE_SPACE}+  { } 
{COMMENTS}      { }
{NOT_COMMENTS}  { return TOKEN_ERROR; }

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

{STRING_VALUE}  { return TOKEN_STRING_LITERAL; }
{BINARY}        { return TOKEN_BINARY_LITERAL; }
{HEXIDECIMAL}   { return TOKEN_HEXIDECIMAL_LITERAL; }
{INTEGER}       { return TOKEN_INTEGER_LITERAL; }
{SCIENTIFIC}    { return TOKEN_DOUBLE_SCIENTIFIC_LITERAL; }
{DOUBLE_VALUE}  { return TOKEN_DOUBLE_LITERAL; }
{CHAR_VALUE}    { return TOKEN_CHAR_LITERAL; }

{IDENTIFIER}    { return TOKEN_IDENTIFIER; }

.               { return TOKEN_ERROR; }

%%
/* User Code */

int yywrap() { return 1; }