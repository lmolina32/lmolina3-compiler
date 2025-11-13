/* type.h: type structure and functions */

#ifndef TYPE_H
#define TYPE_H

#include <stdio.h>
#include <stdbool.h>

/* Forward Declaration */

typedef struct Param_list Param_list;
typedef struct Expr Expr;

/* Structure */

typedef enum {
	TYPE_VOID,
	TYPE_BOOLEAN,
	TYPE_CHARACTER,
	TYPE_INTEGER,
	TYPE_DOUBLE,
	TYPE_STRING,
	TYPE_ARRAY,
	TYPE_CARRAY,
	TYPE_AUTO,
	TYPE_FUNCTION,
} type_t;

typedef struct Type Type;

struct Type {
	type_t kind;			// type of data type 
	Param_list *params;		// arg list for data types 
	Type *subtype;			// subtypes for functions and arrays 
	Expr *arr_len;			// get array len 
	Type *orig_type; 		// original type 
	Symbol *symbol;			// symbol associated with type
};

/* Macros */

#define ILLEGAL_KIND_EQUALITY(t) \
    ((t) == TYPE_VOID || (t) == TYPE_FUNCTION || \
     (t) == TYPE_ARRAY || (t) == TYPE_CARRAY || (t) == TYPE_AUTO)

/* Functions */

Type	     *type_create(type_t kind, Type *subtype, Param_list *params, Expr *arr_len);
void		  type_destroy(Type *t);
void          type_print(Type *t, FILE *stream);
Type         *type_copy(Type *t);
bool 		  type_equals(Type *a,  Type *b);
bool		  type_valid_return(Type *a);

#endif
