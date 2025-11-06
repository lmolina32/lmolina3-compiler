/* param_list.c: param_list structure functions */

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
	}

	type_destroy(a->type);
	symbol_destroy(a->symbol);
	param_list_destroy(a->next);
	free(a);
}

/**
 * Prints a parameter list to stdout.
 * @param a The parameter list to print
 **/
void param_list_print(Param_list *a){
	if (!a) return;

	printf(" %s:", a->name);
	type_print(a->type);

	if (a->next) {
		putchar(',');
		param_list_print(a->next);
	} else {
		putchar(' ');
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
        fprintf(stderr, "resolver error: Redeclaring the same parameter Identifier%s\n", a->name);
        stack.status = 1;
    } else {
        scope_bind(a->name, a->symbol);    
    }

    param_list_resolve(a->next);
}
