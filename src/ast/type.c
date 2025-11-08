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
#include <stdbool.h>

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
	t->params = NULL;
	type_destroy(t->subtype); 
	t->subtype = NULL;
	expr_destroy(t->arr_len);
	t->arr_len = NULL;
	free(t);
}

/**
 * Prints a type representation to stdout.
 * @param t The type to print
 **/
void type_print(Type *t, FILE *stream){
	if (!t) return; 
	
	switch(t->kind){
		case TYPE_VOID:
			fprintf(stream," void");
			break;
		case TYPE_BOOLEAN:
			fprintf(stream," boolean");
			break;
		case TYPE_CHARACTER:
			fprintf(stream," char");
			break;
		case TYPE_INTEGER:
			fprintf(stream," integer");
			break;
		case TYPE_DOUBLE:
			fprintf(stream," double");
			break;
		case TYPE_STRING:
			fprintf(stream," string");
			break;
		case TYPE_ARRAY:
			if (t->arr_len) {
				fprintf(stream," array [");
				expr_print(t->arr_len, stdout);
				putchar(']');
			} else {
				fprintf(stream," array []");
			}
			break;
		case TYPE_CARRAY:
			if (t->arr_len) {
				fprintf(stream," carray [");
				expr_print(t->arr_len, stdout);
				fprintf(stream, "]");
			} else {
				fprintf(stream," carray []");
			}
			break;
		case TYPE_AUTO:
			fprintf(stream," auto");
			break;
		case TYPE_FUNCTION:
			fprintf(stream," function");
			type_print(t->subtype, stream);
			fprintf(stream, "(");
			param_list_print(t->params, stream);
			fprintf(stream, ")");
			return;
	}

	type_print(t->subtype, stream);

}

/**
 * Create a deep copy of type structure 
 * @param    t      Type structure to create deep copy 
 * @return   returns pointer to deep copy of Type structure, otherwise NULL 
 **/
Type* type_copy(Type *t){
    if (!t) return NULL;
    Type *new_t = type_create(t->kind, type_copy(t->subtype), param_list_copy(t->params), expr_copy(t->arr_len));
	new_t->arr_len = expr_copy(t->arr_len);
	return new_t;
}


/**
 * Compares two type structures and ensures they are both equal to each other. 
 * @param	a	ptr to struct to compare another type struct with 
 * @param	b   ptr to struct to compare another type strcut with 
 * @return  true if both type structs are equal, otherwise false
 */
bool type_equals(Type *a,  Type *b){
	// NOTE: might need to compare expr_arr 
	if (!a && !b) return true;
	if (!a || !b) return false;
	if (a->kind != b->kind) return false;
	if (!type_equals(a->subtype, b->subtype)) return false;
	if (!param_list_equals(a->params, b->params)) return false;
	return true;
}


/**
 * Compares Type structure to see if it is has a valid return type 
 * @param 	a 		ptr to Type structure to check valid return 
 * @return 	true if valid return type, otherwise false
 */
bool type_valid_return(Type *a){
	if (!a) return false;
	if (a->kind != TYPE_FUNCTION) return false;
	if (!a->subtype) return false;
	type_t return_type = a->subtype->kind;
	if (return_type == TYPE_FUNCTION || return_type == TYPE_ARRAY || return_type == TYPE_CARRAY) return false;
	return true;
}