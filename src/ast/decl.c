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
    new_d->owner = 1;
    return new_d;
}

/**
 * Checks functions prototype consistency -> check that return types and parameter types are the same 
 * @param   d       ptr to decl to check prototype consistency 
 */
static void decl_resolve_typecheck_functions(Decl *d){
    // Case 2a: functions returns don't match 
    if (!type_equals(d->type->subtype,d->symbol->type->subtype)){
        fprintf(stderr, "Resolver error: Function return type mismatch.\n");
        fprintf(stderr, "\tExpected:\n\t\t");
        type_print(d->symbol->type, stderr);
        fprintf(stderr, "\n\tActual:\n\t\t");
        type_print(d->type, stderr);
        fprintf(stderr, "\n");
        b_ctx.typechecker_errors++;
    }

    // Case 2b: functions parameters don't match 
    if (!param_list_equals(d->type->params, d->symbol->type->params)){
        fprintf(stderr, "Resolver error: Parameter list mismatch for function '%s'.\n", d->name);
        fprintf(stderr, "\tExpected parameters:\n\t\t");
        param_list_print(d->symbol->type->params, stderr);
        fprintf(stderr, "\n\tDefined parameters:\n\t\t");
        param_list_print(d->type->params, stderr);
        fprintf(stderr, "\n");
        b_ctx.typechecker_errors++;    
    }
}


/**
 * Helper function to perform decl_resolve, this resolves all the non function edge cases
 * @param   d       ptr to decl 
 * @param   sym     symbol structure assigned to decl 
 */
static void decl_resolve_non_functions(Decl *d, Symbol *sym){
    expr_resolve(d->type->arr_len);
    if (sym) {
        // Error: Redeclaration or Variable/Function name collision
        if (sym->type->kind == TYPE_FUNCTION){
            fprintf(stderr, "Resolver error: Declaring Identifier with function name '%s'\n", d->name);
        } else{
            fprintf(stderr, "Resolver error: Redeclaring an Identifier '%s' in the same scope\n", d->name);
        }
        b_ctx.resolver_errors += 1;
        symbol_destroy(d->symbol);
        d->symbol = sym;
    } else{
        d->owner = 1;
        // Case 1b: if array decl with init braces pass symbol to it 
        if ((d->type->kind == TYPE_ARRAY || d->type->kind == TYPE_CARRAY || d->type->kind == TYPE_AUTO) && d->value && d->value->kind == EXPR_BRACES){
            d->value->symbol = d->symbol;
        }
        scope_bind(d->name, d->symbol);    
    }
}

/**
 * Helper function to perform decl_resolve, this resolves all the function edge cases
 * @param   d       ptr to decl 
 * @param   sym     symbol structure assigned to decl 
 */
static void decl_resolve_functions(Decl *d, Symbol *sym){
    if ((d->type->kind == TYPE_FUNCTION) && !d->code){
        // 1 if function prototype 
        // 0 if not function prototype -> function definition if decl of TYPE_FUNCTION
        d->symbol->func_decl = 1;
    }

    int is_prototype = d->symbol->func_decl; 

    if (sym){
        int sym_is_prototype = sym->func_decl; // symbol is function prototype
        // Error: Function name conflicts with non-function symbol
        if (sym->type->kind != TYPE_FUNCTION){
            fprintf(stderr, "Resolver error: Reusing Identifier '%s' for function name\n", d->name);
            b_ctx.resolver_errors += 1;
            symbol_destroy(d->symbol);
            d->symbol = sym;
        // Case 2a: New definition (not prototype) AND existing symbol is a prototype
        } else if (!is_prototype && sym_is_prototype){
            sym->func_decl = 0;     // function has been initialized 
            symbol_destroy(d->symbol);
            d->symbol = sym;
            decl_resolve_typecheck_functions(d);
        // Case 2b: New definition AND existing symbol is already a definition
        } else if (!is_prototype && !sym_is_prototype){
            fprintf(stderr, "Resolver error: redefinition of '%s'\n", d->name);
            b_ctx.resolver_errors += 1;
            symbol_destroy(d->symbol);
            d->symbol = sym;
            decl_resolve_typecheck_functions(d);
        // Case 2c: New prototype AND existing symbol is already defined
        } else if (is_prototype && !sym_is_prototype){
            fprintf(stderr, "Resolver Warning: '%s' prototype already defined, using the first declaration as reference\n", d->name);
            symbol_destroy(d->symbol);
            d->symbol = sym;
            decl_resolve_typecheck_functions(d);
        // Case 2d: New prototype AND existing symbol is also a prototype
        } else if (is_prototype && sym_is_prototype){ 
            fprintf(stderr, "Resolver Warning: '%s' prototype already defined, using the first declaration as reference\n", d->name);
            symbol_destroy(d->symbol);
            d->symbol = sym;
            decl_resolve_typecheck_functions(d);
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

/**
 * Performs name resolution for declarations
 * @param   d       Declaration structure to perform name resolution
 **/
void decl_resolve(Decl *d){
    if (!d) return;

    d->symbol = symbol_create(scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL, d->type, d->name);

    expr_resolve(d->value);

    Symbol *sym = scope_lookup_current(d->name);
    
    // case 1: Non function declarations (integers, Arrays, ...) 
    if (d->type->kind != TYPE_FUNCTION){
        decl_resolve_non_functions(d, sym);
    // case 2: Function Declaration (Prototypes or Definitions)
    } else {  
        decl_resolve_functions(d, sym);
    }

    decl_resolve(d->next);
}

/**
 * Perform type checking for a declaration that is not a function.
 * @param d   Pointer to the declaration to type check.
 */
static void decl_typecheck_non_functions(Decl *d) {
    Type *t = NULL;
    if (d->value){
        t = expr_typecheck(d->value);
        // Case 1a: decl is auto, assign value returned
        if (d->type->kind == TYPE_AUTO){
            // case 1a-1: typechecker returns void cannot infer the type
            if (t->kind == TYPE_VOID || t->kind == TYPE_AUTO){
                fprintf(stderr, "typechecker error: Declaration '%s' cannot infer type of (", d->name);
                type_print(t, stderr);
                fprintf(stderr, " )\n");
                b_ctx.typechecker_errors++;
            // case 1a-2: typechecker replaces auto with inferred type 
            } else {
                fprintf(stdout, "typechecker resolved: '%s' type set to (", d->name);
                type_print(t, stdout);
                fprintf(stdout, " )\n");
                type_destroy(d->type);
                type_destroy(d->symbol->type);
                d->type = type_copy(t);
                d->symbol->type = type_copy(t);
            }
        }

        // Case 1b: types don't match throw errors
        if (t && t->kind != d->type->kind){
            fprintf(stderr, "typechecker error: Cannot assign value of type");
            type_print(t, stderr);
            fprintf(stderr, " to variable '%s' of type ", d->name);
            type_print(d->type, stderr);
            fprintf(stderr, ".\n");
            b_ctx.typechecker_errors++;
        }

        // Case 1c: Global variable is not a constant value (e.g not Literal)
        if (d->symbol->kind == SYMBOL_GLOBAL){
            if (!expr_is_literal(d->value->kind)){
                fprintf(stderr, "typechecker error: Global variable '%s' must be initialized with a constant value, (",d->name);
                expr_print(d->value, stderr);
                fprintf(stderr, ") is not constant.\n");
                b_ctx.typechecker_errors++;
            }
        }

        // Case 1d: Local array is initialized with '{}', which are not allowed
        if (d->symbol->kind == SYMBOL_LOCAL){
            if ((d->type->kind == TYPE_ARRAY || d->type->kind == TYPE_CARRAY) && d->value->kind == EXPR_BRACES){
                fprintf(stderr, "typechecker error: Local variable '%s' cannot have an array initializer '{}'\n", d->name);
                b_ctx.typechecker_errors++;
            }
        }
        
        type_destroy(t);
    }
    t = d->type;
    // iterate through the array types to ensure they are literals 
    while (t){
        if ((t->kind == TYPE_ARRAY || t->kind == TYPE_CARRAY)){
            // case 2a: array length is not integer 
            if (t->arr_len && t->arr_len->kind != EXPR_INT_LIT){
                fprintf(stderr, "typechecker error: Array size must be constant 'integer literal', non-constant expression (");
                expr_print(t->arr_len, stderr);
                fprintf(stderr, ") used.\n");
                b_ctx.typechecker_errors++; 
            // case 2b: array init is less than 0 throw error
            } else if (t->arr_len && t->arr_len->literal_value <= 0){
                fprintf(stderr, "typechecker error: Array size must be larger than 0 for '%s'\n", d->name);
                b_ctx.typechecker_errors++; 
            }
        }
        t = t->subtype;
    }
    if (d->type->kind == TYPE_AUTO && !d->value){
        fprintf(stderr, "typechecker error: Declarations with type ( auto ) must be initialized with a value, %s is not initialized\n", d->name);
        b_ctx.typechecker_errors++; 
    }
}

/**
 * Perform type checking for a declaration that is not a function.
 * @param d   Pointer to the declaration to type check.
 */
static void decl_typecheck_functions(Decl *d){
    bool res = false;
    // Case 2a: return type auto was set to new return type in previous typecheck
    if (d->type->subtype->kind == TYPE_AUTO && d->symbol->type->subtype->kind != TYPE_AUTO){
        d->type->subtype->kind = d->symbol->type->subtype->kind;
    }

    // Case 2b: check if function returns valid type (handled by parser)
    if (!type_valid_return(d->type->subtype)){
        fprintf(stderr, "typechecker error: Cannot assign");
        type_print(d->type->subtype, stderr);
        fprintf(stderr, " as function return type\n");
        b_ctx.typechecker_errors++;
    }

    // Case 2c: function params cannot be of type auto or void or functions (handled by parser)
    if (!param_list_valid_type(d->type->params)){
        fprintf(stderr, "typechecker error: Invalid type for parameters in function '%s'\n", d->name);
        fprintf(stderr, "\tDeclared Parameters: \n\t\t");
        param_list_print(d->type->params, stderr);
        fprintf(stderr,"\n\tParameters cannot be of type 'void' or 'auto'\n");
        b_ctx.typechecker_errors++;
    }

    // Case 2d: check if function has a return if non-void
    if (d->code){
        res = stmt_typecheck(d->code);
        if (!res && d->type->subtype->kind != TYPE_VOID){
            fprintf(stderr, "typechecker error: control reaches end of non-void function '%s'\n", d->name);
            b_ctx.typechecker_errors++;
        }
        // return type auto was set in stmt_typecheck, update decl
        if (d->type->subtype->kind == TYPE_AUTO && d->symbol->type->subtype->kind != TYPE_AUTO){
            d->type->subtype->kind = d->symbol->type->subtype->kind;
        }
    }
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

    // Case 1: typecheck variable declarations 
    if (d->type->kind != TYPE_FUNCTION){
        decl_typecheck_non_functions(d);
    // Case 2: typecheck function declarations (definitions & prototypes)
    } else {
        decl_typecheck_functions(d);
    }
    decl_typecheck(d->next);
}