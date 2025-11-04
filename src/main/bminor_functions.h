/* bminor_functions.h */

#ifndef BMINOR_FUNCTIONS_H
#define BMINOR_FUNCTIONS_H

#include "decl.h"

#include <stdio.h>

/* Globals */

extern FILE   *yyin;
extern int     yylex();
extern char   *yytext; 
extern int     yyparse();
extern int     yyrestart();
extern int     yylex_destroy();
extern Decl *root;

/* Functions */

int     encode(char *file_name);
int     scan(char *file_name);
int     parse(char *file_name);
int     pretty_print(char *file_name);
int     resolve(char *file_name);

#endif 
