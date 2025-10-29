/* symbol.c: symbol structure functions */

#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
/* Functions */

/**
 * Creates a new symbol table entry.
 * @param kind The symbol scope (SYMBOL_LOCAL, SYMBOL_PARAM, or SYMBOL_GLOBAL)
 * @param type The type of the symbol
 * @param name The identifier name
 * @return Pointer to the newly created Symbol structure
 **/
Symbol* symbol_create(symbol_t kind, Type *type, const char *name){
    Symbol *symbol = safe_calloc(sizeof(Symbol), 1);
    symbol->kind = kind;
    symbol->type = type;
    symbol->name = safe_strdup(name);
    return symbol;
}

/**
 * Frees the Symbol struct 
 * @param s pointer to the Symbol struct 
 */
void symbol_destroy(Symbol *s){
    if (!s) return;
    if (s->name) {
        free(s->name);
    }

    type_destroy(s->type);
    free(s);
}