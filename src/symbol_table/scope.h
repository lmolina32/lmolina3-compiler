/* scop .h: scope structure and functions */

#ifndef SCOPE_H
#define SCOPE_H

#include <stdio.h>

/* forward declaration */

typedef struct Symbol Symbol;

/* Strcutres */ 

typedef struct Symbol_node Symbol_node;

struct Symbol_node {
    struct hash_table *hashmap;
    Symbol_node *next; 
    int params;
    int local;
};

typedef struct Symbol_stack Symbol_stack;

struct Symbol_stack {
    Symbol_node *top;
    int size; 
};

/* Global Variables */

extern Symbol_stack stack;

/* function s*/

void    scope_enter();
void    scope_exit();
int     scope_level();
void    scope_bind( const char *name, Symbol *sym );
Symbol *scope_lookup( const char *name );
Symbol *scope_lookup_current( const char *name );

#endif
