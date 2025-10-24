/* param_list.c: param_list structure functions */

#include "param_list.h"
#include "utils.h"

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
 * Prints a parameter list to stdout.
 * @param a The parameter list to print
 **/
void param_list_print(Param_list *a){
	return NULL;
}