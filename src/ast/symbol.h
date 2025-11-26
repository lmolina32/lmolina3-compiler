/* symbol.h: symbol structure and functions */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h> 
#include <stdbool.h>

/* Forward Declaration */

typedef struct Type Type;

/* Macros */

#define 	MAX_NAME 256

/* Structure */

typedef enum {
	SYMBOL_LOCAL,
	SYMBOL_PARAM,
	SYMBOL_GLOBAL,
    SYMBOL_LEN,
} symbol_t;

typedef struct Symbol Symbol;

struct Symbol {
	symbol_t kind;				// Type of Declaration
	Type *type;					// Type structure associated with Decl
	char *name;					// Name associated with Decl 
	int which;					// The positional location when the Decl was defined
	int func_decl;				// Prototype flag: 1-> Prototype, 0-> Not Prototype 
	Symbol *prototype_def;		// Prototype definition symbol struct 
};


extern const char *sym_to_str[SYMBOL_LEN];

/* Functions */

Symbol 	   *symbol_create(symbol_t kind, Type *type, const char *name);
void		symbol_destroy(Symbol *s);
Symbol     *symbol_copy(Symbol *s);
const char *symbol_codegen(Symbol *s);

#endif
