/* type.c: type structure functions */

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
 * Creates a new type structure.
 * @param kind The type category (e.g., TYPE_INTEGER, TYPE_FUNCTION)
 * @param subtype For arrays, the element type; for functions, the return type (NULL if not applicable)
 * @param params For function types, the parameter list (NULL otherwise)
 * @return Pointer to the newly created Type structure
 **/
Type* type_create(type_t kind, Type *subtype, Param_list *params, Expr *arr_len){
	Type *type = safe_calloc(sizeof(Type), 1);
	type->kind = kind;
	type->subtype = subtype;
	type->params = params;
	type->arr_len = arr_len;
	return type;
}

/**
 * Frees the Type structure 
 * @param t pointer to t struct 
 **/
void type_destroy(Type *t){
	if (!t) return; 
	param_list_destroy(t->params); 
	type_destroy(t->subtype); 
	expr_destroy(t->arr_len);
	free(t);
}

/**
 * Prints a type representation to stdout.
 * @param t The type to print
 **/
void type_print(Type *t){
	if (!t) return; 
	
	switch(t->kind){
		case TYPE_VOID:
			printf(" void");
			break;
		case TYPE_BOOLEAN:
			printf(" boolean");
			break;
		case TYPE_CHARACTER:
			printf(" char");
			break;
		case TYPE_INTEGER:
			printf(" integer");
			break;
		case TYPE_DOUBLE:
			printf(" double");
			break;
		case TYPE_STRING:
			printf(" string");
			break;
		case TYPE_ARRAY:
			if (t->arr_len) {
				printf(" array [");
				expr_print(t->arr_len);
				putchar(']');
			} else {
				printf(" array []");
			}
			break;
		case TYPE_CARRAY:
			if (t->arr_len) {
				printf(" carray [");
				expr_print(t->arr_len);
				putchar(']');
			} else {
				printf(" carray []");
			}
			break;
		case TYPE_AUTO:
			printf(" auto");
			break;
		case TYPE_FUNCTION:
			printf(" function");
			type_print(t->subtype);
			putchar('(');
			param_list_print(t->params);
			putchar(')');
			return;
	}

	type_print(t->subtype);

}

/**
 * Create a deep copy of type structure 
 * @param    t      Type structure to create deep copy 
 * @return   returns pointer to deep copy of Type structure, otherwise NULL 
 **/
Type* type_deep_copy(Type *t){
    if (!t) return NULL;
Type*		  type_create(type_t kind, Type *subtype, Param_list *params, Expr * arr_len);
    return type_create(t->kind, type_deep_copy(t->subtype), param_list_deep_copy(t->params), expr_deep_copy(t->arr_len));
}
