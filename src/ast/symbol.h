/* symbol.h: symbol structure and functions */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h> 

/* Forward Declaration */

typedef struct Type Type;

/* Structure */

typedef enum {
	SYMBOL_LOCAL,
	SYMBOL_PARAM,
	SYMBOL_GLOBAL,
    SYMBOL_LEN,
} symbol_t;

typedef struct Symbol Symbol;

struct Symbol {
	symbol_t kind;
	Type *type;
	char *name;
	int which;
	int func_decl;
};


extern const char *sym_to_str[SYMBOL_LEN];

/* Functions */

Symbol* 	symbol_create(symbol_t kind, Type *type, const char *name);
void		symbol_destroy(Symbol *s);
Symbol*     symbol_copy(Symbol *s);

#endif
