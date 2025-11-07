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

    if (d->value){      // print decl expression 
        printf(" = ");
        expr_print(d->value);
        printf(";\n");
    } else if (d->code){ // function, print body 
        printf(" = ");

        stmt_print(d->code, 0);
        print_indent(indent);
    } else {            // decl with no expression or body
        printf(";\n");
    }

    decl_print(d->next, 0);

}

/**
 * Creates a deep copy of for the declaration structure 
 * @param   d       decl structure to deep copy 
 * @return  ptr to the deep copy of the decl structure, otherwise NULL. 
 */
Decl *decl_copy(Decl *d){
    if (!d) return NULL;
    Decl *new_d = decl_create(d->name, type_copy(d->type), expr_copy(d->value), stmt_copy(d->code), decl_copy(d->next));
    new_d->symbol = symbol_copy(d->symbol);
    return new_d;
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

    Symbol *sym = scope_lookup_current(d->name);

    // function declaration  
    if ((d->type->kind == TYPE_FUNCTION) && !d->code){
        d->symbol->func_decl = 1;
    }

    // resolve for non function decls 
    if (d->type->kind != TYPE_FUNCTION){
        if (sym && sym->type->kind != TYPE_FUNCTION){
            fprintf(stderr, "resolver error: Redeclaring an Identifier '%s' in the same scope\n", d->name);
            stack.status = 1;
        } else if (sym && sym->type->kind == TYPE_FUNCTION){
            fprintf(stderr, "resolver error: Declaring Identifier with function name '%s'\n", d->name);
            stack.status = 1;
        } else {
            scope_bind(d->name, d->symbol);    
        }
    } else { // resolve for function decls 
        // TODO: not every decl will have its own reference
        if (sym && sym->type->kind != TYPE_FUNCTION){
            fprintf(stderr, "resolver error: Reusing Identifier '%s' for function name\n", d->name);
            stack.status = 1;
        } else if (sym && !d->symbol->func_decl && sym->func_decl){       // func init && func decl
            sym->func_decl = 0;     // function has been initialized 
            symbol_destroy(d->symbol);
            d->symbol = sym;
        } else if (sym && !d->symbol->func_decl && !sym->func_decl){      // func as already bee init so error
            fprintf(stderr, "resolver error: redefinition of '%s'\n", d->name);
            stack.status = 1;
        } else if (sym && d->symbol->func_decl && !sym->func_decl){
            // prototype def again + func definition already defined
            sym->prototype_def = d->symbol; 
        } else if (sym && d->symbol->func_decl && sym->func_decl){ 
           // throw error in typechecking if prototype don't match 
           fprintf(stderr, "Resolver Warning: '%s' prototype already defined, using the first declaration as reference\n", d->name);
           symbol_destroy(d->symbol);
           d->symbol = sym;
        } else {
            scope_bind(d->name, d->symbol);    
        }

        if (d->code){
            scope_enter();
            param_list_resolve(d->type->params);
            scope_enter();
            stmt_resolve(d->code);
            scope_exit();
            scope_exit();
        }
    }

    decl_resolve(d->next);
}
