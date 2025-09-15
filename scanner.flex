/* Definitions */
%{
#include "tokens.h"
%}

WHITE_SPACE     [ \t\r\n]
IDENTIFIER      [a-zA-z_][a-zA-Z_0-9]*


/* Rules */
%%

{WHITE_SPACE}+  { }
{IDENTIFIER}    { return TOKEN_IDENTIFIER; }
.               { return TOKEN_ERROR; }

%%
/* User Code */

int yywrap() { return 1; }