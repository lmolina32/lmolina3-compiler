/* decl.c: decl structure functions */

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
    if (d->owner) symbol_destroy(d->symbol);
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
    type_print(d->type, stdout);

    if (d->value){      // print decl expression 
        printf(" = ");
        expr_print(d->value, stdout);
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
            b_ctx.resolver_errors += 1;
            symbol_destroy(d->symbol);
            d->symbol = sym;
        } else{
            d->owner = 1;
            // Case 1b: if array decl with init braces pass symbol to it 
            if ((d->type->kind == TYPE_ARRAY || d->type->kind == TYPE_CARRAY) && d->value && d->value->kind == EXPR_BRACES){
                d->value->symbol = d->symbol;
            }
            scope_bind(d->name, d->symbol);    
        }
    // case 2: Function Declaration (Prototypes or Definitions)
    } else {  

        if (sym){
            int sym_is_prototype = sym->func_decl; // symbol is function prototype

            // Error: Function name conflicts with non-function symbol
            if (sym->type->kind != TYPE_FUNCTION){
                fprintf(stderr, "resolver error: Reusing Identifier '%s' for function name\n", d->name);
                b_ctx.resolver_errors += 1;
                symbol_destroy(d->symbol);
                d->symbol = sym;
            // Case 2a: New definition (not prototype) AND existing symbol is a prototype
            } else if (!is_prototype && sym_is_prototype){
                sym->func_decl = 0;     // function has been initialized 
                symbol_destroy(d->symbol);
                d->symbol = sym;
            // Case 2b: New definition AND existing symbol is already a definition
            } else if (!is_prototype && !sym_is_prototype){
                fprintf(stderr, "resolver error: redefinition of '%s'\n", d->name);
                b_ctx.resolver_errors += 1;
                symbol_destroy(d->symbol);
                d->symbol = sym;
            // Case 2c: New prototype AND existing symbol is already defined
            } else if (is_prototype && !sym_is_prototype){
                fprintf(stderr, "Resolver Warning: '%s' prototype already defined, using the first declaration as reference\n", d->name);
                symbol_destroy(d->symbol);
                d->symbol = sym;
            // Case 2d: New prototype AND existing symbol is also a prototype
            } else if (is_prototype && sym_is_prototype){ 
                fprintf(stderr, "Resolver Warning: '%s' prototype already defined, using the first declaration as reference\n", d->name);
                symbol_destroy(d->symbol);
                d->symbol = sym;
            }
        } else{
            d->owner = 1;
            scope_bind(d->name, d->symbol);    
        }

        // Resolve for function body 
        if (!sym) sym = scope_lookup_current(d->name);
        if (d->code){
            scope_enter();
            param_list_resolve(d->type->params);
            scope_enter();
            d->code->func_sym = sym;
            stmt_resolve(d->code);
            scope_exit();
            scope_exit();
        }
    }

    decl_resolve(d->next);
}

/**
 * Perform typechecking on the decl structure ensuring compatibility between decls
 * @param   d       decl structure to perform typechecking
 */
void decl_typecheck(Decl *d){
    if (!d) return;
    if (!d->type){
        b_ctx.typechecker_errors += 1;
        fprintf(stderr, "%s is not attached to type structure\n", d->name);
        return;
    }
    if (!d->symbol) {
        b_ctx.typechecker_errors += 1;
        fprintf(stderr, "%s is not attached to symbol structure\n", d->name);
        return;
    }

    Type *t = NULL;
    bool res = false;
    // Case 1: typecheck variable declarations 
    if (d->type->kind != TYPE_FUNCTION){
        if (d->value){
            t = expr_typecheck(d->value);

            // Case 1a: types don't match throw errors
            if (t && t->kind != d->type->kind){
                fprintf(stderr, "typechecker error: Cannot assign value of type");
                type_print(t, stderr);
                fprintf(stderr, " to variable '%s' of type ", d->name);
                type_print(d->type, stderr);
                fprintf(stderr, ".\n");
                b_ctx.typechecker_errors++;
            }

            // Case 1b: Global variable is not a constant value (e.g not Literal)
            if (d->symbol->kind == SYMBOL_GLOBAL){
                if (!expr_is_literal(d->value->kind)){
                    fprintf(stderr, "typechecker error: Global variable '%s' must be initialized with a constant value, (",d->name);
                    expr_print(d->value, stderr);
                    fprintf(stderr, ") is not constant.\n");
                    b_ctx.typechecker_errors++;
                }
            }

            // Case 1c: Local array is initialized with '{}', which are not allowed
            if (d->symbol->kind == SYMBOL_LOCAL){
                if ((d->type->kind == TYPE_ARRAY || d->type->kind == TYPE_CARRAY) && d->value->kind == EXPR_BRACES){
                    fprintf(stderr, "typechecker error: Local variable '%s' cannot have an array initializer '{}'\n", d->name);
                    b_ctx.typechecker_errors++;
                }

            }

            // case 1d: array size initializer must be constant 
            if ((d->type->kind == TYPE_ARRAY || d->type->kind == TYPE_CARRAY)){
                if (d->type->arr_len && d->type->arr_len->kind != EXPR_INT_LIT){
                   fprintf(stderr, "typechecker error: Array size must be constant 'integer literal', non-constant expression (");
                   expr_print(d->type->arr_len, stderr);
                   fprintf(stderr, ") used.\n");
                   b_ctx.typechecker_errors++; 
                }
            }
            type_destroy(t);
        }
    // Case 2: typecheck function declarations (definitions & prototypes)
    } else {
        // Case 2a: check if function returns valid type (handled by parser)
        if (!type_valid_return(d->type)){
            fprintf(stderr, "typechecker error: Cannot assign");
            type_print(t->subtype, stderr);
            fprintf(stderr, " as function return type\n");
            b_ctx.typechecker_errors++;
        }

        // Case 2b: functions returns don't match 
        if (d->type->kind != d->symbol->type->kind){
            fprintf(stderr, "typechecker error: Function return type mismatch.\n");
            fprintf(stderr, "\tDeclared:\n\t\t");
            type_print(d->symbol->type, stderr);
            fprintf(stderr, "\n\tActual:\n\t\t");
            type_print(d->type, stderr);
            fprintf(stderr, "\n");
            b_ctx.typechecker_errors++;
        }

        // Case 2c: functions parameters don't match 
        if (!param_list_equals(d->type->params, d->symbol->type->params)){
            fprintf(stderr, "typechecker error: Parameter list mismatch for function '%s'.\n", d->name);
            fprintf(stderr, "\tDeclared parameters:\n\t\t");
            param_list_print(d->symbol->type->params, stderr);
            fprintf(stderr, "\n\tDefined parameters:\n\t\t");
            param_list_print(d->type->params, stderr);
            fprintf(stderr, "\n");
            b_ctx.typechecker_errors++;    
        }

        // Case 2d: function params cannot be of type auto or void or functions
        if (!param_list_valid_type(d->type->params)){
            fprintf(stderr, "typechecker error: Invalid type for parameters in function '%s'\n", d->name);
            fprintf(stderr, "\tDeclared Parameters: \n\t\t");
            param_list_print(d->type->params, stderr);
            fprintf(stderr,"\n\tParameters cannot be of type 'void' or 'auto'\n");
            b_ctx.typechecker_errors++;
        }

        // Case 2e: check if function has a return if non-void
        if (d->code){
            res = stmt_typecheck(d->code);
            if (!res && d->type->subtype->kind != TYPE_VOID){
                fprintf(stderr, "typechecker error: control reaches end of non-void function '%s'\n", d->name);
                b_ctx.typechecker_errors++;
            }
        }
    }
    decl_typecheck(d->next);
}