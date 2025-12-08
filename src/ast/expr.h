/* expr.h: expr structure and functions */

#ifndef EXPR_H
#define EXPR_H

#include <stdio.h>
#include <stdbool.h>

/* Forward Declaration */
typedef struct Symbol Symbol;
typedef struct Type Type;

/* Structure */

typedef enum {
	EXPR_ADD,					//	addition +
	EXPR_SUB,					//	subtraction -
	EXPR_MUL,					//	multiplication *
	EXPR_DIV,					//  division  /
	EXPR_ASSIGN,				// 	assignment =  
	EXPR_OR,					//  logical or ||
	EXPR_AND,					//  logical and  &&
	EXPR_EQ,					//  comparison equal  ==
	EXPR_NOT_EQ,				//  comparison not equal  !=
	EXPR_LT,					//  comparison less than  <
	EXPR_LTE,					//  comparison less than or equal  <=
	EXPR_GT,					//  comparison greater than > 
	EXPR_GTE,					//  comparison greater than or equal >=
	EXPR_REM,					//  remainder %
	EXPR_EXPO,					//  exponentiation ^  
	EXPR_NOT,					//  logical not !
	EXPR_NEGATION,			    //  negation  -
	EXPR_ARR_LEN,			    //  array len #
	EXPR_INCREMENT,			    //  increment ++ 
	EXPR_DECREMENT,			    //  decrement -- 
	EXPR_GROUPS,				//  grouping ()
	EXPR_FUNC,					//  function call f()
	EXPR_ARGS,					//  function arguments a, b, c, d 
	EXPR_INDEX,					//  subscripts, indexes a[0] or a[b]
	EXPR_BRACES,				//  braces {}
	EXPR_INT_LIT,				//  integer literal 21321 
	EXPR_HEX_LIT,				//  hexadecimal literal 0x2123
	EXPR_BIN_LIT,				//  binary literal 0b1010
	EXPR_DOUBLE_LIT,			//  double literal 123131 
	EXPR_DOUBLE_SCIENTIFIC_LIT, //  double scientific literal 6e10 
	EXPR_CHAR_LIT, 				//  char literal 'a'
	EXPR_STR_LIT,				//  string literal "hello"
	EXPR_BOOL_LIT,				//  boolean literal 'true' 'false'
	EXPR_IDENT,					//  identifier    my_function 
	EXPR_COUNT
} expr_t;

typedef struct Expr Expr;

struct Expr {
	expr_t kind;  					// expr kind from above (e.g +)
	Expr *left;						// left child of expr kind (e.g 5+4, left child is 5)
	Expr *right;					// right child of expr kind (e.g 5+4, right child is 4)

	char *name;						// identifier (e.g a[b], a is name)
	int literal_value;				// literal value (char, int, hex, bin, bool)
	double double_literal_value;	// double lit val (double & double scientific)
	char *string_literal;			// string literal 
	Symbol *symbol;					// include const, vars, and funcs 
	int reg;						// scratch register associated with expr
	const char *label;						// label associated with expression 
};

/* Functions */

Expr   *expr_create(expr_t kind, Expr *left, Expr *right);
void	expr_destroy(Expr *e);
Expr   *expr_create_name(const char *n);
Expr   *expr_create_integer_literal(int c);
Expr   *expr_create_boolean_literal(int c);
Expr   *expr_create_double_literal(double c);
Expr   *expr_create_char_literal(char *c);
Expr   *expr_create_string_literal(const char *str);
void 	expr_print(Expr *e, FILE *stream);
Expr   *expr_copy(Expr *e);
void    expr_resolve(Expr *e);
Type   *expr_typecheck(Expr *e);
bool	expr_is_literal(expr_t type);
void	expr_codegen(Expr *e, FILE *f);


#endif
