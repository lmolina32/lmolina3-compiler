/* decl.c: decl structure functions */

#include "decl.h"
#include "utils.h"

/* Functions */

/**
 * Creates a new declaration node.
 * @param   name        The identifier name for this declaration
 * @param   type        The type information for this declaration
 * @param   value       Optional initialization expression (NULL if none)
 * @param   code        Optional function body for function declarations (NULL if none)
 * @param   next        Pointer to the next declaration in a list (NULL if last)
 * @return  Pointer to the newly created Decl structure
 **/
Decl* decl_create(const char *name, Type *type, Expr *value, Stmt *code, Decl *next){
    Decl *decl = safe_calloc(sizeof(Decl), 1);
    decl->name = safe_strdup(name);
    decl->type = type;
    decl->value = value;
    decl->code = code;
    decl->next = next;
	return decl;
}

/**
 * Prints a declaration node and its contents to stdout.
 * @param   d           The declaration to print
 * @param   indent      The indentation level for formatting output
 **/
void decl_print(Decl *d, int indent){
	return NULL;
}