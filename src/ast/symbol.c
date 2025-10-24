/* symbol.c: symbol structure functions */

#include "symbol.h"
#include "utils.h"

/* Functions */

/**
 * Creates a new symbol table entry.
 * @param kind The symbol scope (SYMBOL_LOCAL, SYMBOL_PARAM, or SYMBOL_GLOBAL)
 * @param type The type of the symbol
 * @param name The identifier name
 * @return Pointer to the newly created Symbol structure
 **/
Symbol* symbol_create(symbol_t kind, Type *type, const char *name){
    Symbol *symbol = safe_callco(sizeof(Symbol), 1);
    symbol->kind = kind;
    symbol->type = type;
    symbol->name = safe_strdup(name);
    return symbol;
}