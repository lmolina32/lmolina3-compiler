/* expr.c: expr structure functions */

#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"
#include "encoder.h"
#include "scope.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

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
	if (e->name) {
		free(e->name);
	}

	if (e->string_literal) {
		free(e->string_literal);
	}

	expr_destroy(e->left);
	expr_destroy(e->right);
	symbol_destroy(e->symbol);
	free(e);
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
void expr_print_with_context(Expr *parent, Expr *child, int is_left){
	if (!child) return; 

	if (child->kind == EXPR_GROUPS){
		Expr *expr_unwrapped = expr_unwrap_groups(child);

		if (parent && expr_need_parens(parent, expr_unwrapped, is_left)){
			putchar('(');
			expr_print(expr_unwrapped);
			putchar(')');
		} else {
			expr_print(expr_unwrapped);
		}
	} else {
		if (parent && expr_need_parens(parent, child, is_left)){
			putchar('(');
			expr_print(child);
			putchar(')');
		} else {
			expr_print(child);
		}
	}
}

/**
 * Prints an expression tree to stdout.
 * @param   e        The expression to print
 **/
void expr_print(Expr *e){
	if (!e) return; 
	char es[BUFSIZ] = {0};

	switch (e->kind){
		case EXPR_ADD:					//	addition +
			expr_print_with_context(e, e->left, 1);
			putchar('+');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_SUB:					//	subtraction -
			expr_print_with_context(e, e->left, 1);
			putchar('-');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_MUL:					//	multiplication *
			expr_print_with_context(e, e->left, 1);
			putchar('*');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_DIV:					//  division  /
			expr_print_with_context(e, e->left, 1);
			putchar('/');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_ASSIGN:				// 	assignment =  
			expr_print_with_context(e, e->left, 1);
			putchar(' ');
			putchar('=');
			putchar(' ');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_OR:					//  logical or ||
			expr_print_with_context(e, e->left, 1);
			putchar('|');
			putchar('|');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_AND:					//  logical and  &&
			expr_print_with_context(e, e->left, 1);
			putchar('&');
			putchar('&');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_EQ:					//  comparison equal  ==
			expr_print_with_context(e, e->left, 1);
			putchar('=');
			putchar('=');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_NOT_EQ:				//  comparison not equal  !=
			expr_print_with_context(e, e->left, 1);
			putchar('!');
			putchar('=');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_LT:					//  comparison less than  <
			expr_print_with_context(e, e->left, 1);
			putchar('<');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_LTE:					//  comparison less than or equal  <=
			expr_print_with_context(e, e->left, 1);
			putchar('<');
			putchar('=');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_GT:					//  comparison greater than > 
			expr_print_with_context(e, e->left, 1);
			putchar('>');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_GTE:					//  comparison greater than or equal >=
			expr_print_with_context(e, e->left, 1);
			putchar('>');
			putchar('=');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_REM:					//  remainder %
			expr_print_with_context(e, e->left, 1);
			putchar('%');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_EXPO:					//  exponentiation ^  
			expr_print_with_context(e, e->left, 1);
			putchar('^');
			expr_print_with_context(e, e->right, 0);
			break;
		case EXPR_NOT:					//  logical not !
			putchar('!');
			expr_print_with_context(e, e->left, 1);
			break;
		case EXPR_NEGATION:			    //  negation  -
			putchar('-');
			expr_print_with_context(e, e->left, 1);
			break;
		case EXPR_ARR_LEN:			    //  array len #
			putchar('#');
			expr_print_with_context(e, e->left, 1);
			break;
		case EXPR_INCREMENT:			//  increment ++ 
			expr_print_with_context(e, e->left, 1);
			putchar('+');
			putchar('+');
			break;
		case EXPR_DECREMENT:			//  decrement -- 
			expr_print_with_context(e, e->left, 1);
			putchar('-');
			putchar('-');
			break;
		case EXPR_GROUPS:				//  grouping ()
			expr_print(expr_unwrap_groups(e));
			break;
		case EXPR_FUNC:					//  function call f()
			expr_print(e->left);
			putchar('(');
			expr_print(e->right);
			putchar(')');
			break;
		case EXPR_ARGS:					//  function arguments a, b, c, d 
			expr_print(e->left);
			if (e->right && e->right->kind == EXPR_ARGS){
				putchar(',');
				expr_print(e->right);
			}
			break;
		case EXPR_INDEX:				//  subscripts, indexes a[0] or a[b]
			expr_print(e->left);
			putchar('[');
			expr_print(e->right);
			putchar(']');
			break;
		case EXPR_BRACES:				//  braces {}
			putchar('{');
			expr_print(e->right);
			putchar('}');	
			break;
		case EXPR_INT_LIT:				//  integer literal 21321 
			printf("%d", e->literal_value);
			break;
		case EXPR_HEX_LIT:				//  hexadecimal literal 0x2123
			printf("%d", e->literal_value);
			break;
		case EXPR_BIN_LIT:				//  binary literal 0b1010
			printf("%d", e->literal_value);
			break;
		case EXPR_DOUBLE_LIT:			//  double literal 123131 
			printf("%lf", e->double_literal_value);
			break;
		case EXPR_DOUBLE_SCIENTIFIC_LIT://  double scientific literal 6e10 
			printf("%lf", e->double_literal_value);
			break;
		case EXPR_CHAR_LIT: 			//  char literal 'a'
			string_encode(e->string_literal, es);
			chomp_quotes(es);
			printf("'%s'", es+1);
			break;
		case EXPR_STR_LIT:				//  string literal "hello"
			string_encode(e->string_literal, es);
			printf("%s", es);
			break;
		case EXPR_BOOL_LIT:				//  boolean literal 'true' 'false'
			printf("%s", e->literal_value ? "true" : "false");
			break;
		case EXPR_IDENT:				//  identifier    my_function 
			printf("%s", e->name);
			break;
		default:
			fprintf(stderr, "Invalid Expression type\n");
			exit(1);
	}

}

/**
 * Crete a deep copy of the expression structure 
 * @param   e       Expression structure to create deep copy of 
 * @return  ptr to expression struct or NULL if unsuccesful
 **/
Expr* expr_deep_copy(Expr *e){
    if (!e) return NULL;
    return expr_create(e->kind, expr_deep_copy(e->left), expr_deep_copy(e->right));
}

/**
 * Perform name resolution on Expression structure 
 * @param   e       Expression structure to perform name resolution
 **/
void expr_resolve(Expr *e){
    if (!e) return;
    
    if (e->kind == EXPR_IDENT){
        e->symbol = symbol_deep_copy(scope_lookup(e->name));
        if (e->symbol){
            if (e->symbol->kind == SYMBOL_GLOBAL){
                printf("resolver: %s resolves to %s %s\n", e->name, sym_to_str[e->symbol->kind], e->symbol->name);            
            } else {
                printf("resolver: %s resolves to %s %d\n", e->name, sym_to_str[e->symbol->kind], e->symbol->which);            
                
            }
        } else {
            printf("resolver error: %s is not defined", e->name);
            stack.status = 1;
        }
    } else {
        expr_resolve(e->left);
        expr_resolve(e->right);
    }
}
