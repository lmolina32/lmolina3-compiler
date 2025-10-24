/* type.c: type structure functions */

#include "type.h"
#include "utils.h"

/* Functions */

/**
 * Creates a new type structure.
 * @param kind The type category (e.g., TYPE_INTEGER, TYPE_FUNCTION)
 * @param subtype For arrays, the element type; for functions, the return type (NULL if not applicable)
 * @param params For function types, the parameter list (NULL otherwise)
 * @return Pointer to the newly created Type structure
 **/
Type* type_create(type_t kind, Type *subtype, Param_list *params){
	Type *type = safe_calloc(sizeof(type), 1);
	type->kind = kind;
	type->subtype = subtype;
	type->params = params;
	return type;
}

/**
 * Prints a type representation to stdout.
 * @param t The type to print
 **/
void type_print(Type *t){
	return NULL;
}