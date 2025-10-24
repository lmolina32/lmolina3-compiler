/* expr.c: expr structure functions */

#include "expr.h"
#include "utils.h"

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
	return NULL;
}
