/* stmt.c: stmt structure functions */

#include "stmt.h"
#include "utils.h"

/* Functions */

/**
 * Creates a new statement node.
 * @param kind The type of statement (e.g., STMT_IF_ELSE, STMT_FOR)
 * @param decl Declaration for STMT_DECL (NULL otherwise)
 * @param init_expr Initialization expression for loops (NULL if not applicable)
 * @param expr Main expression for the statement (NULL if not applicable)
 * @param next_expr Next expression for loops (NULL if not applicable)
 * @param body Body statement(s) (NULL if not applicable)
 * @param else_body Else clause for if-else statements (NULL if not applicable)
 * @param next Pointer to the next statement in a sequence (NULL if last)
 * @return Pointer to the newly created Stmt structure
 **/
Stmt* stmt_create(stmt_t kind, Decl *decl, Expr *init_expr, Expr *expr, Expr *next_expr, Stmt *body, Stmt *else_body, Stmt *next){
	Stmt *stmt = safe_calloc(sizeof(Stmt), 1);
	stmt->kind = kind;
	stmt->decl = decl;
	stmt->init_expr = init_expr;
	stmt->expr = expr;
	stmt->next_expr = next_expr;
	stmt->body = body;
	stmt->else_body = else_body;
	stmt->next = stmt->next;
	return stmt;
}

/**
 * Prints a statement tree to stdout.
 * @param s The statement to print
 * @param indent The indentation level for formatting output
 **/
void stmt_print(Stmt *s, int indent){
	return NULL;
}