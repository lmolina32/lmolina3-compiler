/* expr.c: expr structure functions */

#include "bminor_context.h"
#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"
#include "encoder.h"
#include "scope.h"
#include "utils.h"
#include "scratch.h"
#include "label.h"
#include "str_lit.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static const int expr_associativity[EXPR_COUNT] = {
	// 0 -> left associative  
	// 1 -> right associative 
	[EXPR_ASSIGN] = 1,
	[EXPR_ARR_LEN] = 1,
	[EXPR_EXPO] = 1, 
	[EXPR_NOT] = 1,
	[EXPR_NEGATION] = 1
};

static const int expr_precedence[EXPR_COUNT] = {
	// lowest precedence = 0
	// highest precedence = 10
	[EXPR_ADD] = 4,					
	[EXPR_SUB] = 4,					
	[EXPR_MUL] = 5,					
	[EXPR_DIV] = 5,					
	[EXPR_ASSIGN] = 0,				
	[EXPR_OR] = 1,					
	[EXPR_AND] = 2,					
	[EXPR_EQ] = 3,					
	[EXPR_NOT_EQ] = 3,				
	[EXPR_LT] = 3,					
	[EXPR_LTE] = 3,					
	[EXPR_GT] = 3,					
	[EXPR_GTE] = 3,					
	[EXPR_REM] = 5,					
	[EXPR_EXPO] = 6,					
	[EXPR_NOT] = 7,					
	[EXPR_NEGATION] = 7,			    
	[EXPR_ARR_LEN] = 8,			    
	[EXPR_INCREMENT] = 9,			    
	[EXPR_DECREMENT] = 9,			    
	[EXPR_GROUPS] = 10,				
	[EXPR_FUNC] = 10,					
	[EXPR_ARGS] = 10,					
	[EXPR_INDEX] = 10,					
	[EXPR_BRACES] = 10,				
	[EXPR_INT_LIT] = 10,				
	[EXPR_HEX_LIT] = 10,				
	[EXPR_BIN_LIT] = 10,				
	[EXPR_DOUBLE_LIT] = 10,			
	[EXPR_DOUBLE_SCIENTIFIC_LIT] = 10, 
	[EXPR_CHAR_LIT] = 10, 				
	[EXPR_STR_LIT] = 10,				
	[EXPR_BOOL_LIT] = 10,				
	[EXPR_IDENT] = 10,					
};

/* Functions */

/**
 * Creates a new expression node with binary operation.
 * @param	kind		The type of expression (e.g., EXPR_ADD, EXPR_SUB)
 * @param	left		The left operand expression
 * @param	right		The right operand expression
 * @return Pointer to the newly created Expr structure
 **/
Expr* expr_create(expr_t kind, Expr *left, Expr *right){
    Expr *expr = safe_calloc(sizeof(Expr), 1);
	expr->kind = kind;
	expr->left = left;
	expr->right = right;
	return expr;
}

/** Frees the Expr struct 
 * @param e pointer to the Expr struct 
 */
void expr_destroy(Expr *e){
	if (!e) return;

	Expr *left = e->left;
	Expr *right = e->right;
	if (e->name) {
		free(e->name);
		e->name = NULL;
	}

	if (e->string_literal) {
		free(e->string_literal);
		e->string_literal = NULL;
	}
	e->left = e->right = NULL;

	free(e);
	expr_destroy(left);
	expr_destroy(right);
}

/**
 * Creates a name/identifier expression node.
 * @param    n          The identifier name
 * @return Pointer to the newly created Expr structure
 **/
Expr* expr_create_name(const char *n){
	Expr* expr_name = safe_calloc(sizeof(Expr), 1);
	expr_name->kind = EXPR_IDENT;
	expr_name->name = safe_strdup(n);
	return expr_name;
}

/**
 * Creates an integer literal expression node.
 * @param   c           The integer value
 * @return Pointer to the newly created Expr structure
 **/
Expr* expr_create_integer_literal(int c){
	Expr* expr_int_lit = safe_calloc(sizeof(Expr), 1);
	expr_int_lit->kind = EXPR_INT_LIT;
	expr_int_lit->literal_value = c;
	return expr_int_lit;
}


/**
 * Creates a boolean literal expression node.
 * @param   c           The boolean value (0 for false, non-zero for true)
 * @return Pointer to the newly created Expr structure
 **/
Expr* expr_create_boolean_literal(int c){
	Expr* expr_bool_lit = safe_calloc(sizeof(Expr), 1);
	expr_bool_lit->kind = EXPR_BOOL_LIT;
	expr_bool_lit->literal_value = c;
	return expr_bool_lit;
}

/**
 * Creates a character literal expression node.
 * @param   c           The character value
 * @return Pointer to the newly created Expr structure
 **/
Expr* expr_create_char_literal(char *c){
	Expr* expr_char_lit = safe_calloc(sizeof(Expr), 1);
	expr_char_lit->kind = EXPR_CHAR_LIT;
	expr_char_lit->string_literal = safe_strdup(c);
	expr_char_lit->literal_value = (int)*c;
	return expr_char_lit;
}

/**
 * Creates a double literal expression node.
 * @param   c           The double value
 * @return Pointer to the newly created Expr structure
 **/
Expr*	expr_create_double_literal(double c){
	Expr* expr_double_lit = safe_calloc(sizeof(Expr), 1);
	expr_double_lit->kind = EXPR_DOUBLE_LIT;
	expr_double_lit->double_literal_value = c;
	return expr_double_lit;
}

/**
 * Creates a string literal expression node.
 * @param   str          The string value
 * @return Pointer to the newly created Expr structure
 */
Expr* expr_create_string_literal(const char *str){
	Expr* expr_str_lit = safe_calloc(sizeof(Expr), 1);
	expr_str_lit->kind = EXPR_STR_LIT;
	expr_str_lit->string_literal= safe_strdup(str);
	return expr_str_lit;
}

/**
 * This function unwraps nested expr_groups chained together 
 * @param e  ptr to expression struct we want to unwrap 
 * @return   ptr to expression struct that is unwrapped of all the expr_groups
 */
Expr *expr_unwrap_groups(Expr *e){
	if (!e) return NULL;

	while (e && e->kind == EXPR_GROUPS && e->left) {
		e = e->left;
	}

	return e;
}

/**
 * Checks if the child needs parenthesis based on precedence or associativity  
 * @param Parent 	ptr of parent or root of the child, holds the operation we are trying to check if child needs parenthesis 
 * @param child		ptr of child we are trying to see if they need parenthesis 
 * @param is_left	1 if left child of parent or 0 if right child of parent 
 * @return			1 if the child needs parenthesis, 0 if they do not
 */
int expr_need_parens(Expr *parent, Expr *child, int is_left){
	if (!child || !parent) return 0;

	int parent_prec = expr_precedence[parent->kind];
	int child_prec = expr_precedence[child->kind];

	// check precedence for child compared to parent
	if (child_prec < parent_prec){
		return 1;
	}

	// check associativity when precedence is the same 
	if ( child_prec == parent_prec){
		
		// if !!!!!a ensures that !(!(!(!a))) is not printed out 
		if (child->kind == EXPR_NOT && parent->kind == EXPR_NOT){
			return 0;
		}

		// if associativity left (e.g eval left to right) and evaluating right child add parens  
		if (expr_associativity[parent->kind] == 0 && !is_left){
			return 1;
		}

		// if associativity is right (e.g eval right to left) and evaluating left child add parens
		if (expr_associativity[parent->kind] == 1 && is_left){
			return 1;
		}
	}

	return 0;
	
}

/**
 * Takes the parent and child and determines if parenthesis are needed them prints them out knowing the context
 * @param parent 	ptr to root or parent of child 
 * @param child		ptr to child of parent (left or right)
 * @param is_left	integer: 1 if left child, 0 if right child 
 */
void expr_print_with_context(Expr *parent, Expr *child, int is_left, FILE *stream){
	if (!child) return; 

	if (child->kind == EXPR_GROUPS){
		Expr *expr_unwrapped = expr_unwrap_groups(child);

		if (parent && expr_need_parens(parent, expr_unwrapped, is_left)){
			fprintf(stream,"(");
			expr_print(expr_unwrapped, stream);
			fprintf(stream,")");
		} else {
			expr_print(expr_unwrapped, stream);
		}
	} else {
		if (parent && expr_need_parens(parent, child, is_left)){
			fprintf(stream,"(");
			expr_print(child, stream);
			fprintf(stream,")");
		} else {
			expr_print(child, stream);
		}
	}
}

/**
 * Prints an expression tree to stdout.
 * @param   e        The expression to print
 **/
void expr_print(Expr *e, FILE *stream){
	if (!e) return; 
	char es[BUFSIZ] = {0};

	switch (e->kind){
		case EXPR_ADD:					//	addition +
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "+");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_SUB:					//	subtraction -
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "-");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_MUL:					//	multiplication *
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "*");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_DIV:					//  division  /
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "/");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_ASSIGN:				// 	assignment =  
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, " ");
			fprintf(stream, "=");
			fprintf(stream, " ");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_OR:					//  logical or ||
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "|");
			fprintf(stream, "|");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_AND:					//  logical and  &&
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "&");
			fprintf(stream, "&");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_EQ:					//  comparison equal  ==
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "=");
			fprintf(stream, "=");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_NOT_EQ:				//  comparison not equal  !=
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "!");
			fprintf(stream, "=");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_LT:					//  comparison less than  <
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "<");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_LTE:					//  comparison less than or equal  <=
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "<");
			fprintf(stream, "=");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_GT:					//  comparison greater than > 
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, ">");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_GTE:					//  comparison greater than or equal >=
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, ">");
			fprintf(stream, "=");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_REM:					//  remainder %
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "%%");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_EXPO:					//  exponentiation ^  
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "^");
			expr_print_with_context(e, e->right, 0, stream);
			break;
		case EXPR_NOT:					//  logical not !
			fprintf(stream, "!");
			expr_print_with_context(e, e->left, 1, stream);
			break;
		case EXPR_NEGATION:			    //  negation  -
			fprintf(stream, " -");
			expr_print_with_context(e, e->left, 1, stream);
			break;
		case EXPR_ARR_LEN:			    //  array len #
			fprintf(stream, "#");
			expr_print_with_context(e, e->left, 1, stream);
			break;
		case EXPR_INCREMENT:			//  increment ++ 
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "+");
			fprintf(stream, "+");
			break;
		case EXPR_DECREMENT:			//  decrement -- 
			expr_print_with_context(e, e->left, 1, stream);
			fprintf(stream, "-");
			fprintf(stream, "-");
			break;
		case EXPR_GROUPS:				//  grouping ()
			expr_print(expr_unwrap_groups(e), stream);
			break;
		case EXPR_FUNC:					//  function call f()
			expr_print(e->left, stream);
			fprintf(stream, "(");
			expr_print(e->right, stream);
			fprintf(stream, ")");
			break;
		case EXPR_ARGS:					//  function arguments a, b, c, d 
			expr_print(e->left, stream);
			if (e->right && e->right->kind == EXPR_ARGS){
				fprintf(stream, ",");
				expr_print(e->right, stream);
			}
			break;
		case EXPR_INDEX:				//  subscripts, indexes a[0] or a[b]
			expr_print(e->left, stream);
			fprintf(stream, "[");
			expr_print(e->right, stream);
			fprintf(stream, "]");
			break;
		case EXPR_BRACES:				//  braces {}
			fprintf(stream, "{");
			expr_print(e->right, stream);
			fprintf(stream, "}");	
			break;
		case EXPR_INT_LIT:				//  integer literal 21321 
			fprintf(stream, "%d", e->literal_value);
			break;
		case EXPR_HEX_LIT:				//  hexadecimal literal 0x2123
			fprintf(stream, "%d", e->literal_value);
			break;
		case EXPR_BIN_LIT:				//  binary literal 0b1010
			fprintf(stream, "%d", e->literal_value);
			break;
		case EXPR_DOUBLE_LIT:			//  double literal 123131 
			fprintf(stream, "%lf", e->double_literal_value);
			break;
		case EXPR_DOUBLE_SCIENTIFIC_LIT://  double scientific literal 6e10 
			fprintf(stream, "%lf", e->double_literal_value);
			break;
		case EXPR_CHAR_LIT: 			//  char literal 'a'
			string_encode(e->string_literal, es);
			chomp_quotes(es);
			fprintf(stream, "'%s'", es+1);
			break;
		case EXPR_STR_LIT:				//  string literal "hello"
			string_encode(e->string_literal, es);
			fprintf(stream, "%s", es);
			break;
		case EXPR_BOOL_LIT:				//  boolean literal 'true' 'false'
			fprintf(stream, "%s", e->literal_value ? "true" : "false");
			break;
		case EXPR_IDENT:				//  identifier    my_function 
			fprintf(stream, "%s", e->name);
			break;
		default:						//  if not defined identifier then error 
			fprintf(stderr, "Invalid Expression type\n");
			exit(1);
	}

}

/**
 * Crete a deep copy of the expression structure 
 * @param   e       Expression structure to create deep copy of 
 * @return  ptr to expression struct or NULL if unsuccessful
 **/
Expr* expr_copy(Expr *e){
    if (!e) return NULL;
    Expr* new_e = expr_create(e->kind, expr_copy(e->left), expr_copy(e->right));
	new_e->name = e->name ? safe_strdup(e->name) : NULL;
	new_e->literal_value = e->literal_value;
	new_e->double_literal_value = e->double_literal_value;
	new_e->string_literal = e->string_literal ? safe_strdup(e->string_literal) : NULL;
	return new_e;
}

/**
 * Perform name resolution on Expression structure 
 * @param   e       Expression structure to perform name resolution
 **/
void expr_resolve(Expr *e){
    if (!e) return;
    if (e->kind == EXPR_IDENT){
        e->symbol = scope_lookup(e->name);
        if (e->symbol){
            if (e->symbol->kind == SYMBOL_GLOBAL){
                printf("resolver: %s resolves to %s %s\n", e->name, sym_to_str[e->symbol->kind], e->symbol->name);
            } else {
                printf("resolver: %s resolves to %s %d\n", e->name, sym_to_str[e->symbol->kind], e->symbol->which);            
            }
        } else {
            printf("resolver error: %s is not defined\n", e->name);
            b_ctx.resolver_errors += 1;
        }
    } else {
        expr_resolve(e->left);
        expr_resolve(e->right);
    }
}

/**
 * Check if both operand types are numeric types (integer or double).
 * @param   lt      left-hand operand type
 * @param   rt      right-hand operand type
 * @return  true if both numeric and same numeric kind, otherwise false
 */
static bool expr_valid_numeric_op(Type *lt, Type *rt){
	return (lt->kind == TYPE_INTEGER && rt->kind == TYPE_INTEGER) ||
		   (lt->kind == TYPE_DOUBLE && rt->kind == TYPE_DOUBLE);
}

/**
 * Check if type structure is numeric type (integer, double)
 * @param 	t		ptr to type structure to check numeric type 
 * @return	true if numeric type, otherwise false. 
 */
static bool expr_is_numeric_type(Type *t){
	if (!t) return false;
	return t->kind == TYPE_INTEGER || t->kind == TYPE_DOUBLE;
}

/**
 * Typecheck arithmetic operators (+, -, *, /, %, ^) for numeric operands.
 * @param   e       expression node representing operator
 * @param   lt      left-hand operand type
 * @param   rt      right-hand operand type
 * @return  resulting numeric type, or integer type on error
 */
static Type *expr_typecheck_arithmetic_op(Expr *e, Type *lt, Type *rt){
	// expects both to be integers or both to be doubles 
	Expr *dummy_e = expr_create(e->kind, 0, 0);
	bool valid = expr_valid_numeric_op(lt, rt);

	if (!valid || ((valid && e->kind == EXPR_REM && lt->kind == TYPE_DOUBLE && rt->kind == TYPE_DOUBLE))){
		fprintf(stderr, "typechecker error: Invalid operand types for '");
		expr_print(dummy_e, stderr);
		fprintf(stderr, "' operator. Got");
		type_print(lt, stderr);
		fprintf(stderr, " and");
		type_print(rt, stderr);
		if (e->kind == EXPR_REM){
			fprintf(stderr, " but expected either (integer, integer).\n");
		} else {
			fprintf(stderr, " but expected either (integer, integer) or (double, double).\n");
		}
		b_ctx.typechecker_errors++;
	}
	expr_destroy(dummy_e);
	return type_create(valid ? lt->kind : TYPE_INTEGER, 0, 0, 0);
}

/**
 * Typecheck unary numeric operators (++, --, -) for numeric operand.
 * @param   e       expression node representing operator
 * @param   lt      operand type
 * @return  resulting numeric type, or integer type on error
 */
static Type *expr_typecheck_unary_numeric_op(Expr *e, Type *lt){
	Expr *dummy_e = expr_create(e->kind, 0, 0);
	bool valid = expr_is_numeric_type(lt);

	if (!valid){
		fprintf(stderr, "typechecker error: Invalid operand type for '");
		expr_print(dummy_e, stderr);
		fprintf(stderr, "' operator. Got");
		type_print(lt, stderr);
		fprintf(stderr, " but expected either (integer) or (double).\n");
		b_ctx.typechecker_errors++;
	}
	expr_destroy(dummy_e);
	return type_create(valid ? lt->kind : TYPE_INTEGER, 0, 0, 0);
}

/**
 * Typecheck assignment operator and ensure left and right types match.
 * @param   e       expression node representing assignment
 * @param   lt      left-hand operand (lvalue) type
 * @param   rt      right-hand operand type
 * @return  type of left-hand operand
 */
static Type *expr_typecheck_assignment(Expr *e, Type *lt, Type *rt){
	Expr *dummy_e = expr_create(e->kind, 0, 0);
	// case 1: check for auto subtype on the left assigned array on the right 
	if ((lt->kind == TYPE_ARRAY && rt->kind == TYPE_ARRAY) || (lt->kind == TYPE_CARRAY || rt->kind == TYPE_CARRAY)){
		// case 1a: check if array types are same and that left subtype is auto
		if (type_arrays_equals(lt, rt)){
			Type *copy_type = rt;
			while (copy_type->subtype){
				copy_type = copy_type->subtype;
			}
		
			// case 1a-1: check if right subtype is auto 
			if (copy_type->kind == TYPE_AUTO){
				fprintf(stderr, "typechecker error: Cannot infer operand types for operator '");
				expr_print(dummy_e, stderr);
				fprintf(stderr, "': both operands base types are 'auto'\n");	
				b_ctx.typechecker_errors++;
			// case 1a-2: valid type assign right array to left array
			} else {
				copy_type = rt;
				type_print(copy_type, stdout); printf("\n");
				Type *base_type = lt->symbol->type;
				Type *dummy_t = lt;
				// get correct base of the right subtype 
				while (dummy_t->subtype){
					copy_type = copy_type->subtype;
					dummy_t = dummy_t->subtype;
				}
				// get base type of left 
				while (base_type && base_type->subtype && base_type->subtype->subtype){
					base_type = base_type->subtype;
				}
				// assign and finish
				type_print(copy_type, stdout); printf("\n");
				type_destroy(base_type->subtype);
				base_type->subtype = type_copy(copy_type);	
				fprintf(stdout, "typechecker resolved: Variable '%s' type set to (", lt->symbol->name);
				type_print(rt, stdout);
				fprintf(stdout, " )\n");
			}

			expr_destroy(dummy_e);
			return type_create(lt->kind, 0, 0, 0);
		}
	}

	// case 2: check left side is identifier or index 
    if (e->left->kind != EXPR_IDENT && e->left->kind != EXPR_INDEX) {
        fprintf(stderr, "typechecker error: Cannot assign to non-lvalue (");
        expr_print(e->left, stderr);
        fprintf(stderr, ")\n");
        b_ctx.typechecker_errors++;
        expr_destroy(dummy_e);
        return type_create(TYPE_INTEGER, 0, 0, 0);
	// case 3: both assignments are auto, can't infer type
    } else if (lt->kind == TYPE_AUTO && rt->kind == TYPE_AUTO){
		fprintf(stderr, "typechecker error: Cannot infer operand types for operator '");
		expr_print(dummy_e, stderr);
		fprintf(stderr, "': both operands are 'auto'\n");
		b_ctx.typechecker_errors++;
	// case 4: left child is auto, assign right type to left child
	} else if (lt->kind == TYPE_AUTO && rt) { 
		if (lt->symbol){
			lt->kind = rt->kind;
			// case 4a: child is array -> traverse array type and set base type to rt
			if (lt->symbol->type->kind == TYPE_ARRAY || lt->symbol->type->kind == TYPE_CARRAY){
				// get array type (e.g array [5] integer -> arr_type = ptr to integer)
				Type *base_type = lt->symbol->type;
				while (base_type && base_type->subtype && base_type->subtype->subtype){
					base_type = base_type->subtype;
				}
				type_destroy(base_type->subtype);
				base_type->subtype = type_copy(rt);
			// case 4b: child is just auto, set the type
			} else {
				type_destroy(lt->symbol->type);
				lt->symbol->type = type_copy(rt);
			}
			fprintf(stdout, "typechecker resolved: Variable '%s' type set to (", lt->symbol->name);
			type_print(rt, stdout);
			fprintf(stdout, " )\n");
		}
	// case 5: the types don't match -> throw error
	} else if (!type_equals(lt, rt)){
		fprintf(stderr, "typechecker error: Invalid operand type for '");
		expr_print(dummy_e, stderr);
		fprintf(stderr, "' operator. Got");
		type_print(lt, stderr);
		fprintf(stderr, " and");
		type_print(rt, stderr);
		fprintf(stderr, " but expected (");
		type_print(lt, stderr);
		fprintf(stderr, ",");
		type_print(lt, stderr);
		fprintf(stderr,").\n");
		b_ctx.typechecker_errors++;
	}
	expr_destroy(dummy_e);
	return type_create(lt->kind, 0, 0, 0);
}

/**
 * Typecheck logical binary operators (||, &&) requiring boolean operands.
 * @param   e       expression node representing logical operator
 * @param   lt      left-hand operand type
 * @param   rt      right-hand operand type
 * @return  boolean type
 */
static Type *expr_typecheck_logical_binary_op(Expr *e, Type *lt, Type *rt){
	Expr *dummy_e = expr_create(e->kind, 0, 0);
	if (lt->kind != TYPE_BOOLEAN || rt->kind != TYPE_BOOLEAN){
		fprintf(stderr, "typechecker error: Invalid operand types for '");
		expr_print(dummy_e, stderr);
		fprintf(stderr, "' operator. Got");
		type_print(lt, stderr);
		fprintf(stderr, " and");
		type_print(rt, stderr);
		fprintf(stderr, " but expected (boolean).\n");
		b_ctx.typechecker_errors++;
	}
	expr_destroy(dummy_e);
	return type_create(TYPE_BOOLEAN, 0, 0, 0);
}

/**
 * Typecheck logical NOT operator (!) requiring boolean operand.
 * @param   e       expression node representing logical NOT
 * @param   lt      operand type
 * @return  boolean type
 */
static Type *expr_typecheck_logical_not(Expr *e, Type *lt){
	Expr *dummy_e = expr_create(e->kind, 0, 0);
	if (lt->kind != TYPE_BOOLEAN){
		fprintf(stderr, "typechecker error: Invalid operand type for '");
		expr_print(dummy_e, stderr);
		fprintf(stderr, "' operator. Got");
		type_print(lt, stderr);
		fprintf(stderr, " but expected (boolean).\n");
		b_ctx.typechecker_errors++;
	}
	expr_destroy(dummy_e);
	return type_create(TYPE_BOOLEAN, 0, 0, 0);
}

/**
 * Typecheck equality operators (==, !=) ensuring valid and matching types.
 * @param   e       expression node representing equality operator
 * @param   lt      left-hand operand type
 * @param   rt      right-hand operand type
 * @return  boolean type
 */
static Type *expr_typecheck_equality_op(Expr *e, Type *lt, Type *rt){
	Expr *dummy_e = expr_create(e->kind, 0, 0);
	if (ILLEGAL_KIND_EQUALITY(lt->kind) || ILLEGAL_KIND_EQUALITY(rt->kind) ||
		(lt->kind != rt->kind)){
		fprintf(stderr, "type error: invalid types for equality operator '");
		expr_print(dummy_e, stderr);
		fprintf(stderr, "'. Left is '");
		type_print(lt, stderr);
		fprintf(stderr,"', right is '");
		type_print(rt, stderr);
		fprintf(stderr, "'. Equality requires matching types and cannot be applied to void, array, or function types.\n");
		b_ctx.typechecker_errors++;
	} 
	expr_destroy(dummy_e);
	return type_create(TYPE_BOOLEAN, 0, 0, 0);
}

/**
 * Typecheck comparison operators (<, <=, >, >=) for numeric operands.
 * @param   e       expression node representing comparison operator
 * @param   lt      left-hand operand type
 * @param   rt      right-hand operand type
 * @return  boolean type
 */
static Type *expr_typecheck_comparison_op(Expr *e, Type *lt, Type *rt){
	Expr *dummy_e = expr_create(e->kind, 0, 0);
	bool valid = expr_valid_numeric_op(lt, rt);
	if (!valid){
		fprintf(stderr, "typechecker error: Invalid operand types for '");
		expr_print(dummy_e, stderr);
		fprintf(stderr, "' operator. Got");
		type_print(lt, stderr);
		fprintf(stderr, " and");
		type_print(rt, stderr);
		fprintf(stderr, ". Expected either (integer, integer) or (double, double).\n");
		b_ctx.typechecker_errors++;
	}
	expr_destroy(dummy_e);
	return type_create(TYPE_BOOLEAN, 0, 0, 0);
}

/**
 * Typecheck array length operator (#), requiring array operand.
 * @param   e       expression node representing length operator
 * @param   lt      operand type
 * @return  integer type
 */
static Type *expr_typecheck_array_length(Expr *e, Type *lt){
	Expr *dummy_e = expr_create(e->kind, 0, 0);
	if (lt->kind != TYPE_ARRAY){
		fprintf(stderr, "typechecker error: '#' operator requires an array, but got");
		type_print(lt, stderr);
		fprintf(stderr, ".\n");
		b_ctx.typechecker_errors++;
	}
	expr_destroy(dummy_e);
	return type_create(TYPE_INTEGER, 0, 0, 0);
}

/**
 * Typecheck a function-call expression.
 * @param  e   The function-call expression node.
 * @param  lt  The type of the function expression (left side of call).
 * @param  rt  The type of the argument list expression (right side of call).
 * @return  A newly allocated Type representing the function's return type.
 *          Returns TYPE_VOID if a type error is detected.
 */
static Type *expr_typecheck_function(Expr *e, Type *lt, Type *rt){
	// Case 1: Calling function on non-function type
	if (lt->kind != TYPE_FUNCTION){
		fprintf(stderr, "typechecker error: Attempted to call a value of type");
        type_print(lt, stderr);
        fprintf(stderr, " which is not a function.\n");
        b_ctx.typechecker_errors++;
		Type *arg_type = NULL;
		Expr *args = e->right;
		while (args){
			arg_type = expr_typecheck(args->left);
			type_destroy(arg_type);
			args = args->right;
		}
		return type_create(TYPE_VOID, 0, 0, 0);
	}

	Symbol *func_def = e->left->symbol;
	Param_list *params = func_def->type->params;
	// Case 2: Passing parameters to function with no parameters 
	Type *arg_type;
	Expr *args = e->right;
	if (!params && rt){
		fprintf(stderr, "typechecker error: Function '%s' takes no parameters, but arguments were provided.\n", func_def->name);
        b_ctx.typechecker_errors++;
		while (args){
			arg_type = expr_typecheck(args->left);
			type_destroy(arg_type);
			args = args->right;
		}
		return type_create(func_def->type->subtype->kind, 0, 0, 0);
	}

	arg_type = NULL;
	args = e->right;
	// Case 3: Check parameters pass match function definition  
	int count = 0;
	while (params && args){
		arg_type = expr_typecheck(args->left);
		// Case 3a: arg_type is auto, resolve
		if (arg_type->kind == TYPE_AUTO && arg_type){
			fprintf(stderr, "typechecker error: Cannot infer auto type from function parameters.\n");
            b_ctx.typechecker_errors++;
		// Case 3b: params don't match
		} else if (!type_equals(params->type, arg_type)){
			fprintf(stderr, "typechecker error: Argument type mismatch in call to '%s'.", func_def->name);
			fprintf(stderr, "\n\tFunction params\n\t\t");
			param_list_print(func_def->type->params, stderr);
			fprintf(stderr, "\n\tExpected for argument %d:\n\t\t", count);
			fprintf(stderr, " %s:", params->name);
            type_print(params->type, stderr);
            fprintf(stderr, "\n\tPassed in for argument %d:\n\t\t", count);
            type_print(arg_type, stderr);
            fprintf(stderr, "\n");
            b_ctx.typechecker_errors++;
		}
		count++;
		type_destroy(arg_type);
		params = params->next;
		args = args->right;
	}

	// Case 4a: Function has more Params than arguments passed
	if (params && !args){
		fprintf(stderr, "typechecker error: Too few arguments in call to '%s'.", func_def->name);
		fprintf(stderr, "\n\tFunction params\n\t\t");
		param_list_print(func_def->type->params, stderr);
		fprintf(stderr, "\n\tNext Expected Param:\n\t\t");
		fprintf(stderr, " %s:", params->name);
		type_print(params->type, stderr);
		fprintf(stderr, "\n");
		b_ctx.typechecker_errors++;
	}

	// Case 4b: Function has more arguments than params in function
	if (!params && args){
		fprintf(stderr, "typechecker error: Too many arguments in call to '%s'.\n", func_def->name);
		fprintf(stderr, "\tExpected Function params\n\t\t");
		param_list_print(func_def->type->params, stderr);
		fprintf(stderr, "\n");
        b_ctx.typechecker_errors++;
		arg_type = NULL;
		args = e->right;
		while (args){
			arg_type = expr_typecheck(args->left);
			type_destroy(arg_type);
			args = args->right;
		}
	}
	
	return type_create(func_def->type->subtype->kind, 0, 0, 0);
}

/**
 * Typecheck array indexing; left operand must be array and index integer.
 * @param   lt      array or carray type to index
 * @param   rt      index expression type
 * @return  copy of array element type, or input type on error
 */
static Type *expr_typecheck_array_index(Type *lt, Type *rt){
	// Case 1: Array index called on array 
	if (lt->kind == TYPE_ARRAY || lt->kind == TYPE_CARRAY){
		// Case 1a: Array idx is not integer throw error
		if (rt->kind != TYPE_INTEGER){
			fprintf(stderr, "typechecker error: Array index must be of type integer, but got");
			type_print(rt, stderr);
			fprintf(stderr, ".\n");
			b_ctx.typechecker_errors++;
		}
		Type *res = type_copy(lt->subtype);
		res->symbol = lt->symbol;	
		return res;
	// Case 2: tried to index on non-array type
	} else {
		fprintf(stderr, "typechecker error: Cannot index value of type");
		type_print(lt, stderr);
		fprintf(stderr, ". Only arrays support indexing.\n");
		b_ctx.typechecker_errors++;
		return type_copy(lt);
	}	
}

/**
 * Typecheck EXPR_BRACES iterating through all the EXPR_ARGS and typechecking the literals
 * @param 	e		ptr to first EXPR_ARG to iterate over
 */
static void expr_typecheck_non_array_nested_braces(Expr *e){
	Type *t = NULL;
	while (e && e->kind == EXPR_ARGS){
		// case 1a: left side is literal expression
		if (e->left && e->left->kind != EXPR_BRACES){
			t = expr_typecheck(e->left);
			type_destroy(t);
		// case 2a: left side is nested brace
		} else {
			expr_typecheck_non_array_nested_braces(e->left);
		}
		e = e->right;
	}
}

/**
 * Infer the array type of an auto array 
 * @param 	e	ptr to EXPR_ARG to infer type 
 */
static Type *expr_typecheck_infer_auto_array(Expr *e){
	int count = 0;
	Type *element = NULL;
	Type *init = NULL;
	bool nested = false;
	while (e && e->kind == EXPR_ARGS){
		// case 1: nested braces traverse down if subtype hasn't been found 
		if (e->left->kind == EXPR_BRACES){
			nested = true;
			if (!element){
				element = expr_typecheck_infer_auto_array(e->left->right);
			}
		// case 2: expression, typecheck expression and assign if subtype hasn't been done 
		} else {
			if (!element){
				init = expr_typecheck(e->left);
				if (init){
					element = type_copy(init);
				}
				type_destroy(init);
			}
		}
		count++;
		e = e->right;
	}
	
	// Set return array 
	if (nested || element){
		Type *array = type_create(TYPE_ARRAY, element, 0, 0);
		array->arr_len = expr_create_integer_literal(count);
		return array;
	}
	return NULL;
}

/** typecheck EXPR_ARG with the array type passed in ensuring correct length and number of braces
 * @param 	e	ptr to EXPR_ARG to iterate over and typecheck
 * @param 	t	ptr to Array type
 */
static void expr_typecheck_nested_braces(Expr *e, Type *t){
	Symbol *symbol = e->symbol;
	int count = 0;
	// get count if array has count (e.g array [5] integer -> count = 5)
	if (t->arr_len){
		count = t->arr_len->literal_value;
	}

	int levels = 0;
	Type *arr_type = t;
	// get array type (e.g array [5] integer -> arr_type = ptr to integer)
	while (arr_type->kind == TYPE_ARRAY || arr_type->kind == TYPE_CARRAY){
		arr_type = arr_type->subtype;
		levels++;
	}

	int curr_count = 0;
	int curr_lvls = 0;
	Type *init_t = NULL;
	while (e && e->kind == EXPR_ARGS){
		// case 1: left side is identifier -> cannot assign non-constant values in init
		if (e->left->kind == EXPR_IDENT){
			fprintf(stderr, "typechecker error: Array '%s' cannot be initialized with non-constant values (%s)\n", symbol->name, e->left->name);
			b_ctx.typechecker_errors++;
		// case 2: left side is literal expression
		} else if (e->left->kind != EXPR_BRACES){
			init_t = expr_typecheck(e->left);
			// case 2b: Initialize type is auto, set new type if valid
			if (arr_type->kind == TYPE_AUTO && init_t){
				if (init_t->kind == TYPE_AUTO || init_t->kind == TYPE_FUNCTION || init_t->kind == TYPE_VOID || init_t->kind == TYPE_ARRAY || init_t->kind == TYPE_CARRAY){
					fprintf(stderr, "typechecker error: Cannot infer array element type from (");
					type_print(init_t, stderr);
					fprintf(stderr, " )\n");
					b_ctx.typechecker_errors++;
				} else {
					arr_type->kind = init_t->kind;
					fprintf(stdout, "typechecker resolved: ( auto ) in array '%s' set to type (", symbol->name);
					type_print(init_t, stdout);
					fprintf(stdout, " )\n");
				}
			
			// case 2b: Initialize type does not match array type 
			} else if (!type_equals(init_t, arr_type)){
				fprintf(stderr, "typechecker error: Array '%s' type mismatch expected (", symbol->name);
				type_print(arr_type, stderr);
				fprintf(stderr, ") but got (");
				type_print(init_t, stderr);
				fprintf(stderr, ")\n");
				b_ctx.typechecker_errors++;
			// case 2c: Expected higher dimension array but got literal throw error
			} else if (curr_lvls){
				fprintf(stderr, "typechecker error: Array '%s' uses non-initializer for array type\n", symbol->name);
				b_ctx.typechecker_errors++;
			}
			
			type_destroy(init_t);
		// case 3: left side is nested brace (EXPR_BRACES)
		} else {
			curr_lvls++;	
			// case 3a: Array Type has higher Dimension, typecheck the nested braces
			e->left->right->symbol = symbol;
			if (t->subtype){
				t->subtype->orig_type = t->orig_type;
				expr_typecheck_nested_braces(e->left->right, t->subtype);
			// case 3b: Braces Initialized to higher dimension then declared -> expr typecheck the rest of the expressions
			} else {
				Type *new_t = type_create(arr_type->kind, 0, 0, expr_create_integer_literal(1));
				new_t->orig_type = t->orig_type;
				expr_typecheck_nested_braces(e->left->right, new_t);
				type_destroy(new_t);
			}
		}
		e = e->right;
		curr_count++;
	}

	// Case 1: Number of elements in the array exceeds the amount allocated
	if (count && count < curr_count){
		fprintf(stderr, "typechecker error: Array '%s' has too many initializers for array [%d] (expected %d, got %d)\n", symbol->name, count, count, curr_count);
		b_ctx.typechecker_errors++;
	// Case 2: Number of elements in the array is short the amount allocated
	} else if (count && count > curr_count){
		fprintf(stderr, "typechecker error: Array '%s' not enough initializers for array [%d] (expected %d, got %d)\n", symbol->name, count, count, curr_count);
		b_ctx.typechecker_errors++;
	// Case 3: Number of elements is not defined, define it;
	} else if (!count && t && !t->arr_len) {
		t->arr_len = expr_create_integer_literal(curr_count);
		fprintf(stdout, "typechecker resolved: Array '%s' set to length %d\n", symbol->name, curr_count);
		fprintf(stdout, "\tFull type:\n\t\t");
		type_print(symbol->type, stdout);
		fprintf(stdout, "\n");
	}
}

/**
 * Typecheck array braces, ensure they maintain all type safety
 * @param 	e		Expression structure of EXPR_BRACES
 * @return	ptr to Type Structure of the array type, otherwise 1D array of integers 
 */
static Type *expr_typecheck_braces(Expr *e){
	Symbol *arr_sym = e->symbol;
	// case 1: braces init is not assigned to array, typecheck initializers;
	if (!e->symbol){
		expr_typecheck_non_array_nested_braces(e->right);
		return expr_typecheck_infer_auto_array(e->right);
	}
	
	// case 2: braces init is assigned to auto 
	e->right->symbol = arr_sym;
	if (e->symbol->type->kind == TYPE_AUTO){
		Type *inferred_arr = expr_typecheck_infer_auto_array(e->right);
		expr_typecheck_nested_braces(e->right, inferred_arr);
		return inferred_arr;
	}
	// case 3: braces init is assigned to array, typecheck initializers + size
	arr_sym->type->orig_type = arr_sym->type;
	expr_typecheck_nested_braces(e->right, arr_sym->type);
	return type_copy(arr_sym->type);
}

/**
 * Return type associated with a literal expression kind.
 * @param   kind    literal expression kind
 * @return  type structure for literal, or NULL if not a literal
 */
static Type *expr_typecheck_literal(expr_t Kind){
	switch (Kind){
		case EXPR_INT_LIT:				//  integer literal 21321 
		case EXPR_HEX_LIT:				//  hexadecimal literal 0x2123
		case EXPR_BIN_LIT:				//  binary literal 0b1010
			return type_create(TYPE_INTEGER, 0, 0, 0);
		case EXPR_DOUBLE_LIT:			//  double literal 123131 
		case EXPR_DOUBLE_SCIENTIFIC_LIT://  double scientific literal 6e10 
			return type_create(TYPE_DOUBLE, 0, 0, 0);
		case EXPR_CHAR_LIT: 			//  char literal 'a'
			return type_create(TYPE_CHARACTER, 0, 0, 0);
		case EXPR_STR_LIT:				//  string literal "hello"
			return type_create(TYPE_STRING, 0, 0, 0);
		case EXPR_BOOL_LIT:				//  boolean literal 'true' 'false'
			return type_create(TYPE_BOOLEAN, 0, 0, 0);
		default:
			return NULL;
	}
}

/**
 * Perform semantic type checking on an expression tree node.
 * @param 	e 		 Pointer to the expression node to typecheck.
 * @return  A newly allocated `struct type *` representing the expression’s
 *         resulting type. Caller takes ownership and must free it with
 *         `type_delete()`. Never returns NULL (TYPE_ERROR on failure).
 */
Type *expr_typecheck(Expr *e){
	if (!e) return NULL;

	Type *lt = expr_typecheck(e->left);
	Type *rt = expr_typecheck(e->right);

	Type *result = NULL;
	switch (e->kind){
		case EXPR_ADD:					//	addition +
		case EXPR_SUB:					//	subtraction -
		case EXPR_MUL:					//	multiplication *
		case EXPR_DIV:					//  division  /
		case EXPR_REM:					//  remainder %
		case EXPR_EXPO:					//  exponentiation ^  
			result = expr_typecheck_arithmetic_op(e, lt, rt);
			break;
		case EXPR_INCREMENT:			//  increment ++ 
		case EXPR_DECREMENT:			//  decrement -- 
		case EXPR_NEGATION:			    //  negation  -
			result = expr_typecheck_unary_numeric_op(e, lt);
			break;
		case EXPR_ASSIGN:				// 	assignment =  
			result = expr_typecheck_assignment(e, lt, rt);
			break;
		case EXPR_OR:					//  logical or ||
		case EXPR_AND:					//  logical and  &&
			result = expr_typecheck_logical_binary_op(e, lt, rt);
			break;
		case EXPR_NOT:					//  logical not !
			result = expr_typecheck_logical_not(e, lt);
			break;
		case EXPR_EQ:					//  comparison equal  ==
		case EXPR_NOT_EQ:				//  comparison not equal  !=
			result = expr_typecheck_equality_op(e, lt, rt);
			break;
		case EXPR_LT:					//  comparison less than  <
		case EXPR_LTE:					//  comparison less than or equal  <=
		case EXPR_GT:					//  comparison greater than > 
		case EXPR_GTE:					//  comparison greater than or equal >=
			result = expr_typecheck_comparison_op(e, lt, rt);
			break;
		case EXPR_ARR_LEN:			    //  array len #
			result = expr_typecheck_array_length(e, lt);
			break;
		case EXPR_GROUPS:				//  grouping ()
			result = type_copy(lt);
			break;
		case EXPR_FUNC:					//  function call f()
			result = expr_typecheck_function(e, lt, rt);
			break;
		case EXPR_ARGS:					//  function arguments a, b, c, d 
			result = type_copy(lt);
			result->symbol = lt->symbol;
			break;
		case EXPR_INDEX:				//  subscripts, indexes a[0] or a[b]
			result = expr_typecheck_array_index(lt, rt);
			break;
		case EXPR_BRACES:				//  braces {}
			result = expr_typecheck_braces(e);
			break;
		case EXPR_INT_LIT:				//  integer literal 21321 
		case EXPR_HEX_LIT:				//  hexadecimal literal 0x2123
		case EXPR_BIN_LIT:				//  binary literal 0b1010
		case EXPR_DOUBLE_LIT:			//  double literal 123131 
		case EXPR_DOUBLE_SCIENTIFIC_LIT://  double scientific literal 6e10 
		case EXPR_CHAR_LIT: 			//  char literal 'a'
		case EXPR_STR_LIT:				//  string literal "hello"
		case EXPR_BOOL_LIT:				//  boolean literal 'true' 'false'
			result = expr_typecheck_literal(e->kind);
			break;
		case EXPR_IDENT:				//  identifier    my_function 
			result = type_copy(e->symbol->type);
			result->symbol = e->symbol;
			break;
		default:
			fprintf(stderr, "Invalid Expression type\n");
			exit(1);
	}

	type_destroy(lt);
	type_destroy(rt);

	return result;
}

/**
 * Checks if an expression type is a literal or brace expression.
 * @param 	type 		The expression type to check
 * @return 	true if the expression is a literal (integer, hex, binary, double,
 *         double scientific, char, string, boolean) or braces expression,
 *         false otherwise
 */
bool expr_is_literal(expr_t type) {
    return type == EXPR_INT_LIT ||
           type == EXPR_HEX_LIT ||
           type == EXPR_BIN_LIT ||
           type == EXPR_DOUBLE_LIT ||
           type == EXPR_DOUBLE_SCIENTIFIC_LIT ||
           type == EXPR_CHAR_LIT ||
           type == EXPR_STR_LIT ||
           type == EXPR_BOOL_LIT ||
           type == EXPR_BRACES;
}

/**
 * Perform code generation on expr structure
 * @param 	e		expr structure to perform code generation 
 * @param 	f		file pointer to output code generation 
 */
void expr_codegen(Expr *e, FILE *f){
	if (!e || !f) return;

	Expr *dummy_e = NULL;
	Type *dummy_t = NULL;
	int int_count = 0;
	int double_count = 0;
	int label = 0;

	switch (e->kind){
		case EXPR_ADD:					//	addition +
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			fprintf(f, "\tADDQ %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
			e->reg = e->right->reg;
			scratch_free(e->left->reg);
			break;
		case EXPR_SUB:					//	subtraction -
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			fprintf(f, "\tSUBQ %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg));
			e->reg = e->left->reg;
			scratch_free(e->right->reg);
			break;
		case EXPR_MUL:					//	multiplication *
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			fprintf(f, "\tMOVQ %s, %%rax\n", scratch_name(e->right->reg));
			fprintf(f, "\tIMUL %s\n", scratch_name(e->left->reg));
			fprintf(f, "\tMOVQ %%rax, %s\n", scratch_name(e->right->reg));
			e->reg = e->right->reg;
			scratch_free(e->left->reg);
			break;
		case EXPR_DIV:					//  division  /
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			fprintf(f, "\tMOVQ %s, %%rax\n", scratch_name(e->left->reg));
			fprintf(f, "\tCQO\n");
			fprintf(f, "\tIDIVQ %s\n", scratch_name(e->right->reg));
			fprintf(f, "\tMOVQ %%rax, %s\n", scratch_name(e->right->reg));
			e->reg = e->right->reg;
			scratch_free(e->left->reg);
			break;
		case EXPR_ASSIGN:				// 	assignment =  
			// case 1a: left side is string -> allocated new str_literal for string assignment 
			if (e->left->symbol && e->left->symbol->type->kind == TYPE_STRING){
				int label = string_label_create();
				e->left->symbol->str_lit = string_alloc(e->right->string_literal, string_label_name(label));
				e->right->symbol = e->left->symbol;
			} 
			if (e->left->kind == EXPR_INDEX){
				expr_codegen(e->left, f);
				scratch_free(e->left->reg);
			}
			expr_codegen(e->right, f);
			// case 1b: left side is array index -> index into array 
			if (e->left->kind == EXPR_INDEX){
				dummy_e = e->left;
				expr_codegen(dummy_e->right, f);
				dummy_e->reg = scratch_alloc();

				if (dummy_e->left->symbol->kind == SYMBOL_GLOBAL){
					fprintf(f, "\tMOVQ $%s, %s\n", symbol_codegen(dummy_e->left->symbol), scratch_name(dummy_e->reg));
				} else {
					fprintf(f, "\tMOVQ %s, %s\n", symbol_codegen(dummy_e->left->symbol), scratch_name(dummy_e->reg));
				}
				fprintf(f, "\tINCQ %s\n", scratch_name(dummy_e->right->reg));
				fprintf(f, "\tMOVQ %s, (%s, %s, 8)\n", scratch_name(e->right->reg), scratch_name(dummy_e->reg), scratch_name(dummy_e->right->reg));
				scratch_free(dummy_e->right->reg);
				scratch_free(dummy_e->reg);
			// case 1c: left side is regular type -> alloc value to type
			} else {
				fprintf(f, "\tMOVQ %s, %s\n", scratch_name(e->right->reg), symbol_codegen(e->left->symbol));
			}
			e->reg = e->right->reg;
			break;
		case EXPR_OR:					//  logical or ||
			label = label_create();
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			fprintf(f, "\tCMPQ $1, %s\n", scratch_name(e->left->reg));
			fprintf(f, "\tJE %s\n", label_name(label));
			fprintf(f, "\tMOVQ %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg));
			fprintf(f, "%s:\n", label_name(label));
			e->reg = e->left->reg;
			scratch_free(e->right->reg);
			break;
		case EXPR_AND:					//  logical and  &&
			label = label_create();
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			fprintf(f, "\tCMPQ $1, %s\n", scratch_name(e->left->reg));
			fprintf(f, "\tJNE %s\n", label_name(label));
			fprintf(f, "\tMOVQ %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg));
			fprintf(f, "%s:\n", label_name(label));
			e->reg = e->left->reg;
			scratch_free(e->right->reg);
			break;
		case EXPR_EQ:					//  comparison equal  ==
			dummy_t = expr_typecheck(e->left);
			if (dummy_t->kind == TYPE_STRING){
				Expr *res_e = expr_create(EXPR_FUNC, expr_create_name("str_equal"), expr_create(EXPR_ARGS, e->left, expr_create(EXPR_ARGS, e->right, 0)));
				expr_codegen(res_e, f);
				e->reg = res_e->reg;
				res_e->right->left = NULL;
				res_e->right->right = NULL;
				expr_destroy(res_e);
			} else {
				label = label_create();
				expr_codegen(e->left, f);
				expr_codegen(e->right, f);
				fprintf(f, "\tCMPQ %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
				fprintf(f, "\tJE %s\n", label_name(label));
				fprintf(f, "\tMOVQ $0, %s\n", scratch_name(e->left->reg));
				label = label_create();
				fprintf(f, "\tJMP %s\n", label_name(label));
				fprintf(f, "%s:\n", label_name(label-1));
				fprintf(f, "\tMOVQ $1, %s\n", scratch_name(e->left->reg));
				fprintf(f, "%s:\n", label_name(label));
				e->reg = e->left->reg;
				scratch_free(e->right->reg);
			}
			type_destroy(dummy_t);
			break;
		case EXPR_NOT_EQ:				//  comparison not equal  !=
			dummy_t = expr_typecheck(e->left);
			if (dummy_t->kind == TYPE_STRING){
				Expr *res_e = expr_create(EXPR_FUNC, expr_create_name("str_not_equal"), expr_create(EXPR_ARGS, e->left, expr_create(EXPR_ARGS, e->right, 0)));
				expr_codegen(res_e, f);
				e->reg = res_e->reg;
				res_e->right->left = NULL;
				res_e->right->right = NULL;
				expr_destroy(res_e);
			} else {
				label = label_create();
				expr_codegen(e->left, f);
				expr_codegen(e->right, f);
				fprintf(f, "\tCMPQ %s, %s\n", scratch_name(e->left->reg), scratch_name(e->right->reg));
				fprintf(f, "\tJE %s\n", label_name(label));
				fprintf(f, "\tMOVQ $1, %s\n", scratch_name(e->left->reg));
				label = label_create();
				fprintf(f, "\tJMP %s\n", label_name(label));
				fprintf(f, "%s:\n", label_name(label-1));
				fprintf(f, "\tMOVQ $0, %s\n", scratch_name(e->left->reg));
				fprintf(f, "%s:\n", label_name(label));
				e->reg = e->left->reg;
				scratch_free(e->right->reg);
			}
			break;
		case EXPR_LT:					//  comparison less than  <
			label = label_create();
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			fprintf(f, "\tCMPQ %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg));
			fprintf(f, "\tJL %s\n", label_name(label));
			fprintf(f, "\tMOVQ $0, %s\n", scratch_name(e->left->reg));
			label = label_create();
			fprintf(f, "\tJMP %s\n", label_name(label));
			fprintf(f, "%s:\n", label_name(label-1));
			fprintf(f, "\tMOVQ $1, %s\n", scratch_name(e->left->reg));
			fprintf(f, "%s:\n", label_name(label));
			e->reg = e->left->reg;
			scratch_free(e->right->reg);
			break;
		case EXPR_LTE:					//  comparison less than or equal  <=
			label = label_create();
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			fprintf(f, "\tCMPQ %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg));
			fprintf(f, "\tJLE %s\n", label_name(label));
			fprintf(f, "\tMOVQ $0, %s\n", scratch_name(e->left->reg));
			label = label_create();
			fprintf(f, "\tJMP %s\n", label_name(label));
			fprintf(f, "%s:\n", label_name(label-1));
			fprintf(f, "\tMOVQ $1, %s\n", scratch_name(e->left->reg));
			fprintf(f, "%s:\n", label_name(label));
			e->reg = e->left->reg;
			scratch_free(e->right->reg);
			break;
		case EXPR_GT:					//  comparison greater than > 
			label = label_create();
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			fprintf(f, "\tCMPQ %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg));
			fprintf(f, "\tJG %s\n", label_name(label));
			fprintf(f, "\tMOVQ $0, %s\n", scratch_name(e->left->reg));
			label = label_create();
			fprintf(f, "\tJMP %s\n", label_name(label));
			fprintf(f, "%s:\n", label_name(label-1));
			fprintf(f, "\tMOVQ $1, %s\n", scratch_name(e->left->reg));
			fprintf(f, "%s:\n", label_name(label));
			e->reg = e->left->reg;
			scratch_free(e->right->reg);
			break;
		case EXPR_GTE:					//  comparison greater than or equal >=
			label = label_create();
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			fprintf(f, "\tCMPQ %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg));
			fprintf(f, "\tJGE %s\n", label_name(label));
			fprintf(f, "\tMOVQ $0, %s\n", scratch_name(e->left->reg));
			label = label_create();
			fprintf(f, "\tJMP %s\n", label_name(label));
			fprintf(f, "%s:\n", label_name(label-1));
			fprintf(f, "\tMOVQ $1, %s\n", scratch_name(e->left->reg));
			fprintf(f, "%s:\n", label_name(label));
			e->reg = e->left->reg;
			scratch_free(e->right->reg);
			break;
		case EXPR_REM:					//  remainder %
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			fprintf(f, "\tMOVQ %s, %%rax\n", scratch_name(e->left->reg));
			fprintf(f, "\tCQO\n");
			fprintf(f, "\tIDIVQ %s\n", scratch_name(e->right->reg));
			fprintf(f, "\tMOVQ %%rdx, %s\n", scratch_name(e->right->reg));
			e->reg = e->right->reg;
			scratch_free(e->left->reg);
			break;
		case EXPR_EXPO:					//  exponentiation ^  
			dummy_e = expr_create(EXPR_FUNC, expr_create_name("integer_power"), \
								  expr_create(EXPR_ARGS, e->left, \
								  expr_create(EXPR_ARGS, e->right, NULL)));
			expr_codegen(dummy_e, f);
			e->reg = dummy_e->reg;
			expr_destroy(dummy_e);
			break;
		case EXPR_NOT:					//  logical not !
			label = label_create();
			expr_codegen(e->left, f);
			fprintf(f, "\tCMPQ $0, %s\n", scratch_name(e->left->reg));
			fprintf(f, "\tJE %s\n", label_name(label));
			fprintf(f, "\tMOVQ $0, %s\n", scratch_name(e->left->reg));
			label = label_create();
			fprintf(f, "\tJMP %s\n", label_name(label));
			fprintf(f, "%s:\n", label_name(label-1));
			fprintf(f, "\tMOVQ $1, %s\n", scratch_name(e->left->reg));
			fprintf(f, "%s:\n", label_name(label));
			e->reg = e->left->reg;
			break;
		case EXPR_NEGATION:			    //  negation  -
			expr_codegen(e->left, f);
			fprintf(f, "\tNEGQ %s\n", scratch_name(e->left->reg));
			e->reg = e->left->reg;
			break;
		case EXPR_ARR_LEN:			    //  array len #
			dummy_e = expr_create(EXPR_ARGS, 0, 0);
			dummy_e->reg = scratch_alloc();
			fprintf(f, "\tMOVQ $0, %s\n", scratch_name(dummy_e->reg));
			e->reg = scratch_alloc();

			if (e->left->symbol->kind == SYMBOL_GLOBAL){
				fprintf(f, "\tMOVQ $%s, %s\n", symbol_codegen(e->left->symbol), scratch_name(e->reg));
			} else {
				fprintf(f, "\tMOVQ %s, %s\n", symbol_codegen(e->left->symbol), scratch_name(e->reg));
			}
			fprintf(f, "\tMOVQ (%s, %s, 8), %s\n", scratch_name(e->reg), scratch_name(dummy_e->reg), scratch_name(e->reg));
			scratch_free(dummy_e->reg);
			expr_destroy(dummy_e);
			break;
		case EXPR_INCREMENT:			//  increment ++ 
			expr_codegen(e->left, f);
			e->reg = scratch_alloc();
			fprintf(f, "\tMOVQ %s, %s\n", scratch_name(e->left->reg), scratch_name(e->reg));
			fprintf(f, "\tINCQ %s\n", scratch_name(e->left->reg));
			if (e->left->kind == EXPR_IDENT){
				fprintf(f, "\tMOVQ %s, %s\n", scratch_name(e->left->reg), symbol_codegen(e->left->symbol));
			}
			scratch_free(e->left->reg);
			break;
		case EXPR_DECREMENT:			//  decrement -- 
			expr_codegen(e->left, f);
			e->reg = scratch_alloc();
			fprintf(f, "\tMOVQ %s, %s\n", scratch_name(e->left->reg), scratch_name(e->reg));
			fprintf(f, "\tDECQ %s\n", scratch_name(e->left->reg));
			if (e->left->kind == EXPR_IDENT){
				fprintf(f, "\tMOVQ %s, %s\n", scratch_name(e->left->reg), symbol_codegen(e->left->symbol));
			}
			scratch_free(e->left->reg);
			break;
		case EXPR_GROUPS:				//  grouping ()
			expr_codegen(e->left, f);
			e->reg = e->left->reg;
			break;
		case EXPR_FUNC:					//  function call f()
			dummy_e = e->right;	
			for (int i = 0; i < MAX_INT_ARGS; i++){
				fprintf(f, "\tPUSHQ %s\n", int_args[i]);
			}
			while (dummy_e){
				if (double_count + int_count > 6){
					fprintf(stderr, "codegen error: Does not Function '%s' has more than 6 arguments, functions with more than 6 arguments are not implemented\n", e->left->name);
					exit(EXIT_FAILURE);
				}
				expr_codegen(dummy_e->left, f);
				dummy_t = expr_typecheck(dummy_e->left);
				switch (dummy_t->kind){
					case TYPE_DOUBLE:
						fprintf(stderr, "codegen error: double type not supported\n");
						exit(EXIT_FAILURE);
						break;
					case TYPE_BOOLEAN:
					case TYPE_CHARACTER:
					case TYPE_INTEGER:
					case TYPE_STRING:
					case TYPE_ARRAY:
					case TYPE_CARRAY:
						fprintf(f, "\tMOVQ %s, %s\n", scratch_name(dummy_e->left->reg), int_args[int_count++]);
						break;
					case TYPE_VOID:
					case TYPE_AUTO:
					case TYPE_FUNCTION:
					default:
						fprintf(stderr, "codegen error: Invalid function parameter\n");
						exit(EXIT_FAILURE);
				}
				scratch_free(dummy_e->left->reg);
				dummy_e = dummy_e->right;
				type_destroy(dummy_t);
			}

			fprintf(f, "\tPUSHQ %%r10\n"
					   "\tPUSHQ %%r11\n"
					   "\tCALL %s\n", e->left->name);
			fprintf(f, "\tPOPQ %%r11\n"
					   "\tPOPQ %%r10\n");
			for (int i = MAX_INT_ARGS -1; i >= 0; i--){
				fprintf(f, "\tPOPQ %s\n", int_args[i]);
			}
			e->reg = scratch_alloc();
			fprintf(f, "\tMOVQ %%rax, %s\n", scratch_name(e->reg));
			fprintf(f, "\tMOVQ $0, %%rax\n");
			break;
		case EXPR_ARGS:					//  function arguments a, b, c, d 
			expr_codegen(e->left, f);
			expr_codegen(e->right, f);
			e->reg = e->left->reg;
			scratch_free(e->right->reg);
			break;
		case EXPR_INDEX:				//  subscripts, indexes a[0] or a[b]
			dummy_e = expr_create(EXPR_FUNC, expr_create_name("check_bounds"), expr_create(EXPR_ARGS, e->left, expr_create(EXPR_ARGS, e->right, NULL)));
			expr_codegen(dummy_e, f);
			scratch_free(dummy_e->reg);
			dummy_e->right->left = NULL;
			dummy_e->right->right->left = NULL;
			expr_destroy(dummy_e);
			expr_codegen(e->right, f);
			e->reg = scratch_alloc();

			if (e->left->symbol->kind == SYMBOL_GLOBAL){
				fprintf(f, "\tMOVQ $%s, %s\n", symbol_codegen(e->left->symbol), scratch_name(e->reg));
			} else {
				fprintf(f, "\tMOVQ %s, %s\n", symbol_codegen(e->left->symbol), scratch_name(e->reg));
			}
			if (e->left->symbol->type->kind == TYPE_ARRAY){
				fprintf(f, "\tINCQ %s\n", scratch_name(e->right->reg));
			}
			fprintf(f, "\tMOVQ (%s, %s, 8), %s\n", scratch_name(e->reg), scratch_name(e->right->reg), scratch_name(e->reg));
			scratch_free(e->right->reg);
			break;
		case EXPR_BRACES:				//  braces {}
			expr_codegen(e->right, f);
			e->reg = e->right->reg;
			break;
		case EXPR_DOUBLE_LIT:			//  double literal 123131 
		case EXPR_DOUBLE_SCIENTIFIC_LIT://  double scientific literal 6e10 
			fprintf(stderr, "codegen error: Double type not supported\n");
			exit(EXIT_FAILURE);
			break;
		case EXPR_STR_LIT:				//  string literal "hello"
			e->reg = scratch_alloc();
			if (e->symbol){
				fprintf(f, "\tMOVQ $%s, %s\n", e->symbol->str_lit->label, scratch_name(e->reg));
			} else {
				label = string_label_create();
				e->label = string_label_name(label);
				string_alloc(e->string_literal, e->label);
				fprintf(f, "\tMOVQ $%s, %s\n", e->label, scratch_name(e->reg));
			}
			break;
		case EXPR_INT_LIT:				//  integer literal 21321 
		case EXPR_HEX_LIT:				//  hexadecimal literal 0x2123
		case EXPR_BIN_LIT:				//  binary literal 0b1010
		case EXPR_CHAR_LIT: 			//  char literal 'a'
		case EXPR_BOOL_LIT:				//  boolean literal 'true' 'false'
			e->reg = scratch_alloc();
			fprintf(f, "\tMOVQ $%d, %s\n", e->literal_value, scratch_name(e->reg));
			break;
		case EXPR_IDENT:				//  identifier    my_function 
			e->reg = scratch_alloc();
			if (e->symbol->type->kind == TYPE_STRING && e->symbol->str_lit){
				fprintf(f, "\tMOVQ $%s, %s\n", e->symbol->str_lit->label, scratch_name(e->reg));
			} else if (e->symbol->kind == SYMBOL_GLOBAL && (e->symbol->type->kind == TYPE_ARRAY || e->symbol->type->kind == TYPE_CARRAY)){
				fprintf(f, "\tMOVQ $%s, %s\n", symbol_codegen(e->symbol), scratch_name(e->reg));
			} else {
				fprintf(f, "\tMOVQ %s, %s\n", symbol_codegen(e->symbol), scratch_name(e->reg));
			}
			break;
		default:
			fprintf(stderr, "codegen error: Unknown expression type\n");
			exit(EXIT_FAILURE);
	}
}
