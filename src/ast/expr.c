/* expr.c: expr structure functions */

#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"
#include "encoder.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

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
Expr* expr_create_char_literal(char c){
	Expr* expr_char_lit = safe_calloc(sizeof(Expr), 1);
	expr_char_lit->kind = EXPR_CHAR_LIT;
	expr_char_lit->literal_value = c;
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
 * Prints an expression tree to stdout.
 * @param   e        The expression to print
 **/
void expr_print(Expr *e){
	if (!e) return; 
	char es[BUFSIZ] = {0};

	switch (e->kind){
		case EXPR_ADD:					//	addition +
			expr_print(e->left);
			putchar('+');
			expr_print(e->right);
			break;
		case EXPR_SUB:					//	subtraction -
			expr_print(e->left);
			putchar('-');
			expr_print(e->right);
			break;
		case EXPR_MUL:					//	multiplication *
			expr_print(e->left);
			putchar('*');
			expr_print(e->right);
			break;
		case EXPR_DIV:					//  division  /
			expr_print(e->left);
			putchar('/');
			expr_print(e->right);
			break;
		case EXPR_ASSIGN:				// 	assignment =  
			expr_print(e->left);
			putchar(' ');
			putchar('=');
			putchar(' ');
			expr_print(e->right);
			break;
		case EXPR_OR:					//  logical or ||
			expr_print(e->left);
			putchar('|');
			putchar('|');
			expr_print(e->right);
			break;
		case EXPR_AND:					//  logical and  &&
			expr_print(e->left);
			putchar('&');
			putchar('&');
			expr_print(e->right);
			break;
		case EXPR_EQ:					//  comparison equal  ==
			expr_print(e->left);
			putchar('=');
			putchar('=');
			expr_print(e->right);
			break;
		case EXPR_NOT_EQ:				//  comparison not equal  !=
			expr_print(e->left);
			putchar('!');
			putchar('=');
			expr_print(e->right);
			break;
		case EXPR_LT:					//  comparison less than  <
			expr_print(e->left);
			putchar('<');
			expr_print(e->right);
			break;
		case EXPR_LTE:					//  comparison less than or equal  <=
			expr_print(e->left);
			putchar('<');
			putchar('=');
			expr_print(e->right);
			break;
		case EXPR_GT:					//  comparison greater than > 
			expr_print(e->left);
			putchar('>');
			expr_print(e->right);
			break;
		case EXPR_GTE:					//  comparison greater than or equal >=
			expr_print(e->left);
			putchar('>');
			putchar('=');
			expr_print(e->right);
			break;
		case EXPR_REM:					//  remainder %
			expr_print(e->left);
			putchar('%');
			expr_print(e->right);
			break;
		case EXPR_EXPO:					//  exponentiation ^  
			expr_print(e->left);
			putchar('^');
			expr_print(e->right);
			break;
		case EXPR_NOT:					//  logical not !
			putchar('!');
			expr_print(e->left);
			break;
		case EXPR_NEGATION:			    //  negation  -
			putchar('-');
			expr_print(e->left);
			break;
		case EXPR_ARR_LEN:			    //  array len #
			putchar('#');
			expr_print(e->left);
			break;
		case EXPR_INCREMENT:			//  increment ++ 
			expr_print(e->left);
			putchar('+');
			putchar('+');
			break;
		case EXPR_DECREMENT:			//  decrement -- 
			expr_print(e->left);
			putchar('-');
			putchar('-');
			break;
		case EXPR_GROUPS:				//  grouping ()
			putchar('(');
			expr_print(e->left);
			putchar(')');
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
			printf("'%c'", e->literal_value);
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
		case EXPR_ARR_ELE:				//  array element 1: 2, 3
			break;
	}

}
