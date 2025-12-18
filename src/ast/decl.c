/* decl.c: decl structure functions */

#include "bminor_context.h"
#include "encoder.h"
#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"
#include "scope.h"
#include "utils.h"
#include "label.h"
#include "scratch.h"
#include "str_lit.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Globals */

const char *int_args[MAX_INT_ARGS] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
const char *double_args[MAX_DOUBLE_ARGS] = {"%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"};

/* Forward declaration of prototypes */

static void decl_resolve_typecheck_functions(Decl *d);
static void decl_resolve_non_functions(Decl *d, Symbol *sym);
static void decl_resolve_functions(Decl *d, Symbol *sym);
static void decl_typecheck_non_functions(Decl *d) ;
static void decl_typecheck_functions(Decl *d);
static void decl_codegen_preprocess_funcs(Decl *d, FILE *f);
static void decl_codegen_funcs(Decl *d, FILE *f);
static void decl_codegen_preprocess_non_funcs(Decl *d, FILE *f);
static void decl_codegen_string(Decl *d, FILE *f);
static void decl_codegen_array(Decl *d, FILE *f);
static void decl_codegen_non_funcs(Decl *d, FILE *f);


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
        d->local = scope_lookup_which();
        scope_exit();
        d->local -= scope_lookup_which();
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
            if (!expr_is_literal(d->value->kind) && !(d->value->kind == EXPR_NEGATION && expr_is_literal(d->value->left->kind))){
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
    bool scope_lvl = d->symbol->kind == SYMBOL_GLOBAL ? true : false;
    // iterate through the array types to ensure they are literals 
    while (t){
        // global scope type cases 
        if (scope_lvl){
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
        // local scope type cases 
        } else {
            // Case 3: check arr_len is of type integer 
            if ((t->kind == TYPE_ARRAY || t->kind == TYPE_CARRAY)){
                Type *dummy_t = expr_typecheck(t->arr_len);
                if (!dummy_t || dummy_t->kind != TYPE_INTEGER){
                    fprintf(stderr, "typechecker error: Array '%s' must have array size of type integer not of type (", d->name);
                    type_print(t, stderr);
                    fprintf(stderr, " )\n");
                    b_ctx.typechecker_errors++; 
                }
                type_destroy(dummy_t);
                
            }
        }
        t = t->subtype;
    }
    // case 1: decl resolve in expr_codegen never updated -> update
    if (!type_equals(d->type, d->symbol->type)){
        type_destroy(d->type);
        d->type = type_copy(d->symbol->type);
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
        // case 2d-1: reaches end of void function with auto type -> set type to auto 
        if (!res && d->type->subtype->kind == TYPE_AUTO){
            fprintf(stdout, "typechecker resolved: function '%s' ( auto ) return type is set to ( void )\n", d->name);
            d->type->subtype->kind = TYPE_AUTO;
            d->symbol->type->subtype->kind = TYPE_AUTO;
        } 
        // case 2d-2: reaches end of non-void function throw a warning. 
        if (!res && d->type->subtype->kind != TYPE_VOID){
            fprintf(stdout, "typechecker warning: control reaches end of non-void function '%s'\n", d->name);
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

/**
 * Preprocessing stage for decl codegen, this walks decl AST and determines if
 * function declarations follow the simplified requirements. 
 *      Simplified requirements:
 *          - No multi-dimensional arrays 
 *          - No double support 
 *          - No function calls with more than 6 arguments 
 * If requirements are not met the function fails 
 * @param   d       function declaration to preprocess 
 * @param   f       file ptr to write errors to 
 */
static void decl_codegen_preprocess_funcs(Decl *d, FILE *f){
    Param_list *params = d->type->params;
    int count = 0;
    while (params){
        type_t type_param = params->type->kind;
        // Case 1a: function has argument double -> not supported
        if (type_param == TYPE_DOUBLE){
            fprintf(stderr, "codegen error: Double type not supported\n");
            fprintf(f, "codegen error: Double type not supported\n");
            exit(EXIT_FAILURE);
        // Case 1b: Function has argument array 
        } else if (type_param == TYPE_ARRAY || type_param == TYPE_CARRAY){
            type_param = params->type->subtype->kind;
            // Case 1b-1: Function has argument of an array with subtype double -> not supported 
            if (type_param == TYPE_DOUBLE){
                fprintf(stderr, "codegen error: Double type not supported for arrays \n");
                fprintf(f, "codegen error: Double type not supported for arrays\n");
                exit(EXIT_FAILURE);
            // Case 1b-2: Function has argument of multi-dim arrays -> not supported 
            } else if (type_param == TYPE_ARRAY || type_param == TYPE_CARRAY){
                fprintf(stderr, "codegen error: Multi-dimensional arrays are not supported\n");
                fprintf(f, "codegen error: Multi-dimensional arrays are not supported\n");
                exit(EXIT_FAILURE);
            }
        }
        params = params->next;
        count++;
    }
    // case 2a: function with too many arguments (more than 6) -> failure not implemented 
    if (count > 6){
        fprintf(stderr, "codegen error: Function '%s' has more than 6 arguments, functions with more than 6 arguments are not implemented\n", d->name);
        fprintf(f, "codegen error: Function '%s' has more than 6 arguments, functions with more than 6 arguments are not implemented\n", d->name);
        exit(EXIT_FAILURE);
    }

    // case 2b: function return type never resolved -> failure cannot implement
    if (d->type->subtype->kind == TYPE_AUTO){
        fprintf(stderr, "codegen error: Auto type never resolved\n");
        fprintf(f, "codegen error: Auto type never resolved\n");
        exit(EXIT_FAILURE);
    }

    // case 2c: function return type is double -> not supported
    if (d->type->subtype->kind == TYPE_DOUBLE){
        fprintf(stderr, "codegen error: Double type not supported\n");
        fprintf(f, "codegen error: Double type not supported\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Generate x86 code for function declarations 
 * @param   d       Function decl to generate x86 code for 
 * @param   f       File ptr to generate code to 
 */
static void decl_codegen_funcs(Decl *d, FILE *f){
    if (!b_ctx.text_flag) {
        b_ctx.data_flag = false;
        b_ctx.text_flag = true;
        fprintf(f, ".text\n");
    }
    fprintf(f, ".global %s\n"
                "%s:\n", d->name, d->name);
    // save stack ptr 
    fprintf(f, "\tPUSHQ %%rbp\n"
                "\tMOVQ  %%rsp, %%rbp\n\n");

    // save arguments 
    int int_count = 0;
    Param_list *params = d->type->params;

    while (params){
        fprintf(f, "\tPUSHQ %s\n", int_args[int_count++]);
        params = params->next;
    }
    
    // create space for locals 
    fprintf(f, "\n\tSUBQ  $%d, %%rsp\n\n", (int_count + d->local) % 2 == 0 ? (d->local+ 1)*8 : d->local*8);

    // save callee-saved registers
    fprintf(f, "\tPUSHQ %%rbx\n"
                "\tPUSHQ %%r12\n"
                "\tPUSHQ %%r13\n"
                "\tPUSHQ %%r14\n"
                "\tPUSHQ %%r15\n\n");

    // generate function code
    stmt_codegen(d->code, f);

    // restore stack 
    fprintf(f, ".%s_epilogue:\n", d->name);
    fprintf(f, "\tPOPQ %%r15\n"
                "\tPOPQ %%r14\n"
                "\tPOPQ %%r13\n"
                "\tPOPQ %%r12\n"
                "\tPOPQ %%rbx\n\n");

    // restore stack pointer, recover base pointer, and return call
    fprintf(f, "\tMOVQ %%rbp, %%rsp\n"
                "\tPOPQ %%rbp\n"
                "\tRET\n");
}

/**
 * Preprocessing stage for decl codegen, this walks decl AST and determines if
 * non-function declarations follow the simplified requirements. 
 *      Simplified requirements:
 *          - No multi-dimensional arrays, no arrays in local scope 
 *          - No double support 
 * If requirements are not met non-function decls it fails code generation  
 * @param   d       Non-function declaration to preprocess 
 * @param   f       file ptr to write errors to 
 */
static void decl_codegen_preprocess_non_funcs(Decl *d, FILE *f){
    // case 2a: declaration is a multi-dimensional array -> failure not implemented 
    if ((d->type->kind == TYPE_ARRAY || d->type->kind == TYPE_CARRAY) && 
        (d->type->subtype->kind == TYPE_ARRAY || d->type->subtype->kind == TYPE_CARRAY)){
        fprintf(stderr, "codegen error: Multi-dimensional arrays are not supported\n");
        fprintf(f, "codegen error: Multi-dimensional arrays are not supported\n");
        exit(EXIT_FAILURE);
    }

    // case 2b: local declaration is an array -> failure not implemented 
    if (d->symbol->kind == SYMBOL_LOCAL && (d->type->kind == TYPE_ARRAY || d->type->kind == TYPE_CARRAY)){
        fprintf(stderr, "codegen error: Arrays at local scope are not implemented\n");
        fprintf(f, "codegen error: Arrays at local scope are not implemented\n");
        exit(EXIT_FAILURE);
    }

    // case 2c: auto never resolved -> failure cannot implement 
    if (d->type->kind == TYPE_AUTO || ((d->type->kind == TYPE_ARRAY || d->type->kind == TYPE_CARRAY) && (d->type->subtype->kind == TYPE_AUTO))){
        fprintf(stderr, "codegen error: Auto type never resolved\n");
        fprintf(f, "codegen error: Auto type never resolved\n");
        exit(EXIT_FAILURE);
    }

    // case 2d: double type not supported -> failure 
    if (d->type->kind == TYPE_DOUBLE){
        fprintf(stderr, "codegen error: Double type not supported\n");
        fprintf(f, "codegen error: Double type not supported\n");
        exit(EXIT_FAILURE);
    }

    // case 2e: array of double type not supported -> failure
    if ((d->type->kind == TYPE_ARRAY || d->type->kind == TYPE_CARRAY) && 
        d->type->subtype->kind == TYPE_DOUBLE){
        fprintf(stderr, "codegen error: Double type not supported for arrays \n");
        fprintf(f, "codegen error: Double type not supported for arrays\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Generates x86 code for string declarations 
 * @param   d       string decl to generate x86 code for 
 * @param   f       File ptr to generate code to 
 */
static void decl_codegen_string(Decl *d, FILE *f){
    symbol_t sym_type = d->symbol->kind;
    // case 1: decl is global variable 
    if (sym_type == SYMBOL_GLOBAL){
        // case 1a: string decl has expression 
        if (d->value){
            // case 1a-1: expression is identifier, assign str_lit to decl
            if (d->value->kind == EXPR_IDENT){
                d->symbol->str_lit = d->value->symbol->str_lit;
            // case 1a-2: expression is literal -> create string label -> allocated str_lit to decl
            } else {
                int label = string_label_create();
                d->symbol->str_lit = string_alloc(d->value->string_literal, string_label_name(label));
            }
        // case 1b: string decl has no expression -> create string label for empty string -> alloc str_lit to decl
        } else {
            int label = string_label_create();
            d->symbol->str_lit = string_alloc("", string_label_name(label));
        }
    // case 2: decl is local variable 
    } else {
        // case 2a: string decl has expression
        if (d->value){
            // case 2a-1: right side is identifier -> pass str_lit of right side to decl 
            if (d->value->kind == EXPR_IDENT){
                d->symbol->str_lit = d->value->symbol->str_lit;
            // case 2a-2: right side is string literal -> create label + str_lit 
            } else {
                int label = string_label_create();
                d->symbol->str_lit = string_alloc(d->value->string_literal, string_label_name(label));
            }
            d->value->symbol = d->symbol;
            expr_codegen(d->value, f);
            fprintf(f, "\tMOVQ %s, %s\n", scratch_name(d->value->reg), symbol_codegen(d->symbol));
            scratch_free(d->value->reg);
        // case 2b: string decl has no expression -> create label + str_lit 
        } else {
            int label = string_label_create();
            d->symbol->str_lit = string_alloc("", string_label_name(label));
            fprintf(f, "\tMOVQ $%s, %s\n", d->symbol->str_lit->label, symbol_codegen(d->symbol));
        }
    }
}

/**
 * Generate x86 code for array declarations 
 * @param   d       Array decl to generate x86 code for
 * @param   f       File ptr to generate code to 
 */
static void decl_codegen_array(Decl *d, FILE *f){
    symbol_t sym_type = d->symbol->kind;
    if (sym_type == SYMBOL_GLOBAL){
        Expr *curr= d->value ? d->value->right : NULL;
        type_t subtype = d->type->subtype->kind;
        int total_len = d->type->arr_len->literal_value;
        int count = 0;

        // create label for array and init the size for type ARRAYS
        fprintf(f, "%s:\n\t.%s", d->name, "quad");
        if (d->type->kind == TYPE_ARRAY){
            fprintf(f, " %d,", total_len);
        }

        // iterate through entries and fill in array 
        while (curr){
            if (count > 0) { fprintf(f, ", "); }
            else { fprintf(f, " "); }

            if (subtype == TYPE_STRING){
                int label = string_label_create();
                const char *label_name = string_label_name(label);
                string_alloc(curr->left->string_literal, label_name);
                fprintf(f, "%s", label_name);
            } else {
                fprintf(f, "%d", curr->left->literal_value);
            }
            curr = curr->right;
            count++;
        }

        // if array init it empty -> init with NULL
        int label = 0;
        while (count < total_len){
            if (count > 0) { fprintf(f, ", "); } 
            else {
                fprintf(f, " ");
                if (subtype == TYPE_STRING){ 
                    label = string_label_create(); 
                    string_alloc("", string_label_name(label));
                }
            }

            if (subtype == TYPE_STRING){
                fprintf(f, "%s", string_label_name(label));
            } else {
                fprintf(f, "0");
            }
            count++;
        }
        fprintf(f, "\n");
    }
}

/**
 * Generate x86 code for non-function declarations 
 * @param   d       Non-Function decl to generate x86 code for 
 * @param   f       File ptr to generate code to 
 */
static void decl_codegen_non_funcs(Decl *d, FILE *f){
    symbol_t sym_type = d->symbol->kind;
    if (sym_type == SYMBOL_GLOBAL && !b_ctx.data_flag){
        b_ctx.data_flag = true;
        b_ctx.text_flag = false;
        fprintf(f, ".data\n");
    }
    switch (d->type->kind){
        case TYPE_BOOLEAN:
        case TYPE_INTEGER:
        case TYPE_CHARACTER:
            // case 1: decl is global variable 
            if (sym_type == SYMBOL_GLOBAL){
                fprintf(f, "%s:\n\t.quad %d\n", d->name, d->value ? d->value->literal_value : 0);
            // case 2: decl is local variable 
            } else {
                // case 2a: local var has assignment -> codegen expr then assign to ident;
                if (d->value){
                    expr_codegen(d->value, f);
                    fprintf(f, "\tMOVQ %s, %s\n", scratch_name(d->value->reg), symbol_codegen(d->symbol));
                    scratch_free(d->value->reg);
                // case 2b: local var has no assignment -> assign 0
                } else {
                    fprintf(f, "\tMOVQ $0, %s\n", symbol_codegen(d->symbol));
                }
            }
            break;
        case TYPE_DOUBLE:
            fprintf(stderr, "codegen error: Double type not supported\n");
            fprintf(f, "codegen error: Double type not supported\n");
            exit(EXIT_FAILURE);
            break;
        case TYPE_STRING:
            decl_codegen_string(d, f);
            break;
        case TYPE_ARRAY:
        case TYPE_CARRAY:
            decl_codegen_array(d, f);
            break;
        default:
            fprintf(stderr, "codegen error: Invalid type in variable declaration\n");
            exit(EXIT_FAILURE);
            break;
    }
}

/**
 * Perform code generation on the decl structure and output to file 
 * @param   d       decl structure to perform code generation 
 * @param   f       file pointer to output code generation 
 */
void decl_codegen(Decl *d, FILE *f){
    if (!d || !f) return;

    // case 1: code generation on function 
    if (d->type->kind == TYPE_FUNCTION){
        decl_codegen_preprocess_funcs(d, f);
        if (d->code){
            decl_codegen_funcs(d, f);
        }
    // case 2: code generation on variable declarations
    } else {
        decl_codegen_preprocess_non_funcs(d, f);
        decl_codegen_non_funcs(d, f);
    }

    decl_codegen(d->next, f);
}
