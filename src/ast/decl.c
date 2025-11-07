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
#include <stdbool.h>

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

    Decl *next = d->next;
    d->next = NULL;

    if (d->name){
        free(d->name);
        d->name = NULL;
    }

    type_destroy(d->type);
    d->type = NULL;
    expr_destroy(d->value);
    d->type = NULL;
    stmt_destroy(d->code);
    d->code = NULL;
    symbol_destroy(d->symbol);
    d->symbol = NULL;
    free(d);
    decl_destroy(next);
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

    d->symbol = symbol_create(scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL, d->type, d->name);

    expr_resolve(d->value);

    Symbol *sym = scope_lookup_current(d->name);
    if ((d->type->kind == TYPE_FUNCTION) && !d->code){
        // 1 if function prototype 
        // 0 if not function prototype -> function definition if decl of TYPE_FUNCTION
        d->symbol->func_decl = 1;
    }

    int is_prototype = d->symbol->func_decl; 
    // case 1: Non function declarations (integers, Arrays, ...) 
    if (d->type->kind != TYPE_FUNCTION){
        if (sym) {
            // Error: Redeclaration or Variable/Function name collision
            if (sym->type->kind == TYPE_FUNCTION){
                fprintf(stderr, "resolver error: Declaring Identifier with function name '%s'\n", d->name);
            } else{
                fprintf(stderr, "resolver error: Redeclaring an Identifier '%s' in the same scope\n", d->name);
            }
            stack.status = 1;
        } else{
            scope_bind(d->name, d->symbol);    
        }
    // case 2: Function Declaration (Prototypes or Definitions)
    } else {  

        if (sym){
            int sym_is_prototype = sym->func_decl; // symbol is function prototype

            // Error: Function name conflicts with non-function symbol
            if (sym->type->kind != TYPE_FUNCTION){
                fprintf(stderr, "resolver error: Reusing Identifier '%s' for function name\n", d->name);
                stack.status = 1;
            // Case 2a: New definition (not prototype) AND existing symbol is a prototype
            } else if (!is_prototype && sym_is_prototype){
                sym->func_decl = 0;     // function has been initialized 
                d->symbol->prototype_def = sym;
                sym->func_init = d->symbol;
            // Case 2b: New definition AND existing symbol is already a definition
            } else if (!is_prototype && !sym_is_prototype){
                fprintf(stderr, "resolver error: redefinition of '%s'\n", d->name);
                stack.status = 1;
            // Case 2c: New prototype AND existing symbol is already defined
            } else if (is_prototype && !sym_is_prototype){
                // Case 2c-1: Prototype and definition defined -> pass on prototype for typechecking 
                if (sym->prototype_def){
                    d->symbol->prototype_def = sym->prototype_def;
                // Case 2c-2: Definition only defined -> pass on def symbol for typechecking 
                } else {
                    d->symbol->prototype_def = sym->func_init;
                }
            // Case 2d: New prototype AND existing symbol is also a prototype
            } else if (is_prototype && sym_is_prototype){ 
                fprintf(stderr, "Resolver Warning: '%s' prototype already defined, using the first declaration as reference\n", d->name);
                d->symbol->prototype_def = sym;
            }
        } else{
            scope_bind(d->name, d->symbol);    
        }

        // Resolve for function body 
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
