/* stmt.h: stmt structure and functions */

#ifndef STMT_H
#define STMT_H

#include <stdio.h>

/* Forward Declaration */

typedef struct Decl Decl;
typedef struct Expr Expr;

/* Structure */

typedef enum {
	STMT_DECL,
	STMT_EXPR,
	STMT_IF_ELSE,
	STMT_FOR,
	STMT_PRINT,
	STMT_RETURN,
	STMT_BLOCK
} stmt_t;

typedef struct Stmt Stmt;

struct Stmt {
	stmt_t kind;		// kind of stmt
	Decl *decl;			// local decl 
	Expr *init_expr;	// expr in for loop -> for(init_expr;;)
	Expr *expr;			// expr in for loop -> for(;expr;)
	Expr *next_expr;	// expr in for loop -> for(;;next_expr)
	Stmt *body;			// if, for, function body 
	Stmt *else_body;	// else body 
	Stmt *next;			// ptr to next stmt
};

/* Function */

Stmt* 		stmt_create(stmt_t kind, Decl *decl, Expr *init_expr, Expr *expr, Expr *next_expr, Stmt *body, Stmt *else_body, Stmt *next);
void		stmt_destroy(Stmt *s);
void 		stmt_print(Stmt *s, int indent);
Stmt*		stmt_copy(Stmt *s);
void        stmt_resolve(Stmt *s);

#endif
