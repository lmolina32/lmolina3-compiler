/* symbol_table.h: symbol_table structure and functions */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABEL_H

#include <stdio.h>

/* forward declaration */
typedef struct Symbol Symbol;

/* function s*/

void    scope_enter();
void    scope_exit();
int     scope_level();
void    scope_bind( const char *name, Symbol *sym );
Symbol *scope_lookup( const char *name );
Symbol *scope_lookup_current( const char *name );

#endif
