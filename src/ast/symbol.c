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

/* Global arrays */
const char *sym_to_str[SYMBOL_LEN] = {
    [SYMBOL_LOCAL] = "local",
    [SYMBOL_PARAM] = "param",
    [SYMBOL_GLOBAL] = "global",
};

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
    symbol->type = type_copy(type);
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
        s->name = NULL;
    }
    if (s->type){
        type_destroy(s->type);
        s->type = NULL;
    }
    
    free(s);
}

/**
 * This creates a deep copy of a symbol structure 
 * @param   s       symbol structure to make deep copy
 * @return  deep copy of symbol structure, otherwise NULL
 **/
Symbol* symbol_copy(Symbol *s){
    if (!s) return NULL;
    Symbol *new_s = symbol_create(s->kind, type_copy(s->type), s->name);
    new_s->which = s->which;
    new_s->func_decl = s->func_decl; 
    return new_s;
}