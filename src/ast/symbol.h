/* symbol.h: symbol structure and functions */

#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"

#include <stdio.h> 

/* Structure */

typedef enum {
	SYMBOL_LOCAL,
	SYMBOL_PARAM,
	SYMBOL_GLOBAL
} symbol_t;

typedef struct Symbol Symbol;

struct Symbol {
	symbol_t kind;
	Type *type;
	char *name;
	int which;
};

/* Functions */

Symbol* 	symbol_create(symbol_t kind, Type *type, const char *name);

#endif
