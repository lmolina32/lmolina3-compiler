/* type.h: type structure and functions */

#ifndef TYPE_H
#define TYPE_H

#include "param_list.h"

#include <stdio.h>

/* Structure */

typedef enum {
	TYPE_VOID,
	TYPE_BOOLEAN,
	TYPE_CHARACTER,
	TYPE_INTEGER,
	TYPE_STRING,
	TYPE_ARRAY,
	TYPE_FUNCTION,
} type_t;

typedef struct Type Type;

struct Type {
	type_t kind;			// type of data type 
	Param_list *params;		// arg list for data types 
	Type *subtype;			// subtypes for functions and arrays 
};

/* Functions */

Type*		  type_create(type_t kind, Type *subtype, Param_list *params);
void          type_print(Type *t);

#endif
