/* param_list.c: param_list structure functions */

#include "bminor_context.h"
#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"
#include "scope.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Functions */

/**
 * Creates a new parameter list node.
 * @param name The parameter name
 * @param type The parameter type
 * @param next Pointer to the next parameter in the list (NULL if last)
 * @return Pointer to the newly created Param_list structure
 **/
Param_list* param_list_create(const char *name, Type *type, Param_list *next){
	Param_list *param_list = safe_calloc(sizeof(Param_list), 1);
	param_list->name = safe_strdup(name);
	param_list->type = type;
	param_list->next = next;
	return param_list;
}

/**
 * Frees the Param_list struct 
 * @param a pointer to the Param_list struct 
 */
void param_list_destroy(Param_list *a){
	if (!a) return;
	if (a->name) {
		free(a->name);
		a->name = NULL;
	}

	type_destroy(a->type);
	a->type = NULL;
	symbol_destroy(a->symbol);
	a->symbol = NULL;

	Param_list *next = a->next;
	a->next = NULL;
	free(a);

	param_list_destroy(next);
}

/**
 * Prints a parameter list to stdout.
 * @param a The parameter list to print
 **/
void param_list_print(Param_list *a, FILE *stream){
	if (!a) return;

	fprintf(stream, " %s:", a->name);
	type_print(a->type, stream);

	if (a->next) {
		fprintf(stderr, ",");
		param_list_print(a->next, stream);
	} else {
		fprintf(stderr, " ");
	}
}

/**
 * Create deepcopy of param list
 * @param  a        Param list struct to deep copy 
 * @return          ptr to deepcopy or Null if unsuccesful 
 **/
Param_list* param_list_copy(Param_list *a){
    if (!a) return NULL;
    Param_list *p = param_list_create(a->name, type_copy(a->type), param_list_copy(a->next));
	p->symbol = symbol_copy(a->symbol);
	return p;
}


/**
 * Perform name resolution for the param_list structure
 * @param   a       Param List sturcutre to perform name resolution
 **/
void param_list_resolve(Param_list *a){
    if (!a) return;
    a->symbol = symbol_create(SYMBOL_PARAM, a->type, a->name);
    
    if (scope_lookup_current(a->name)){
        fprintf(stderr, "resolver error: Redeclaring the same parameter Identifier %s\n", a->name);
		b_ctx.resolver_errors += 1;
    } else {
        scope_bind(a->name, a->symbol);    
    }

    param_list_resolve(a->next);
}

/**
 * Compares two param_list structures and ensures that they are equal to each other 
 * @param 	a		ptr to struct a that will be compared to b
 * @param 	b 		ptr to struct b that will be compared to a
 * @return  true if param_lists are equal, otherwise false
 */
bool param_list_equals(Param_list *a, Param_list *b){
	// NOTE: might have to add symbols to be compared 
	// NOTE: might not need to compare names
	if (!a && !b) return true;
	if (!a || !b) return false;
	if (!type_equals(a->type, b->type)) return false;
	if (!param_list_equals(a->next, b->next)) return false;
	return true;
}

/**
 * Parameters can be of any type except void or auto per the spec. Ensure valid type for each param
 * @param 	a 		ptr to struct a which types will be checked
 */
bool param_list_valid_type(Param_list *a){
	if (!a) return true;
	if (!a->type){
		fprintf(stderr, "Param %s is not assigned a type\n", a->name);
		return false;
		b_ctx.typechecker_errors++;
	}
	if (a->type->kind == TYPE_VOID || a->type->kind == TYPE_AUTO || a->type->kind == TYPE_FUNCTION) return false;
	if (!param_list_valid_type(a->next)) return false;
	return true;
}