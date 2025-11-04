/* decl.c: decl structure functions */

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
 * Frees the Decl Struct 
 * @param d pointer to the Decl struct 
 */
void decl_destroy(Decl *d){
    if (!d) return;
    if (d->name){
        free(d->name);
    }

    type_destroy(d->type);
    expr_destroy(d->value);
    stmt_destroy(d->code);
    symbol_destroy(d->symbol);
    decl_destroy(d->next);
    free(d);
}
/**
 * Prints a declaration node and its contents to stdout.
 * @param   d           The declaration to print
 * @param   indent      The indentation level for formatting output
 **/
void decl_print(Decl *d, int indent){
    if (!d) { return; }
    print_indent(indent); 
    printf("%s:", d->name);
    type_print(d->type);

    if (d->value){
        printf(" = ");
        expr_print(d->value);
        printf(";\n");
    } else if (d->code){
        printf(" = ");

        stmt_print(d->code, 0);
        print_indent(indent);
    } else {
        printf(";\n");
    }

    decl_print(d->next, 0);

}

/**
 * Performs name resolution for declarations
 * @param   d       Declaration structure to perform name resolution
 **/
void decl_resolve(Decl *d){
    if (!d) return;

    symbol_t sym_type = scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;

    d->symbol = symbol_create(sym_type, d->type, d->name);
    expr_resolve(d->value);

    if (scope_lookup_current(d->name)){
        fprintf(stderr, "resolver: resolver error: Redeclaring an Identifier in the same scope %s\n", d->name);
        stack.status = 1;
    } else {
        scope_bind(d->name, d->symbol);    
    }

    if (d->code){
        scope_enter();
        param_list_resolve(d->type->params);
        stmt_resolve(d->code);
        scope_exit();
    }
    decl_resolve(d->next);
}
