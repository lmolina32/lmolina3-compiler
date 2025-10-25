/* stmt.c: stmt structure functions */

#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

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
	stmt->next = next;
	return stmt;
}

/**
 * Frees the Stmt struct 
 * @param s pointer to Stmt struct 
 */
void stmt_destroy(Stmt *s){
	if (!s) return;
	decl_destroy(s->decl);
	expr_destroy(s->init_expr);
	expr_destroy(s->expr);
	expr_destroy(s->next_expr);
	stmt_destroy(s->body);
	stmt_destroy(s->else_body);
	stmt_destroy(s->next);
	free(s);
}

/**
 * Prints a statement tree to stdout.
 * @param s The statement to print
 * @param indent The indentation level for formatting output
 **/
void stmt_print(Stmt *s, int indent){
	if (!s) return; 
	switch (s->kind){
		case STMT_DECL:
			decl_print(s->decl, indent);
			break;
		case STMT_EXPR:
			print_indent(indent);
			expr_print(s->expr);
			printf(";\n");
			break;
		case STMT_IF_ELSE:
			print_indent(indent);
			printf("if (");
			expr_print(s->expr);
			printf("){\n");
			stmt_print(s->body, indent + 1);
			print_indent(indent);
			putchar('}');
			if (s->else_body){
				printf(" else {\n");
				stmt_print(s->else_body, indent + 1);
				print_indent(indent);
				printf("}\n");
			} else {
				putchar('\n');
			}
			break;
		case STMT_FOR:
			print_indent(indent);
			printf("for (");
			expr_print(s->init_expr);
			putchar(';');
			expr_print(s->expr);
			putchar(';');
			expr_print(s->next_expr);
			printf("){\n");
			stmt_print(s->body, indent + 1);
			print_indent(indent);
			printf("}\n");
			break;
		case STMT_PRINT:
			print_indent(indent);
			printf("print ");
			expr_print(s->expr);
			printf(";\n");
			break;
		case STMT_RETURN:
			print_indent(indent);
			printf("return");
			if (s->expr){
				expr_print(s->expr);
			}
			printf(";\n");
			break;
		case STMT_BLOCK:
			stmt_print(s->body, indent);
			break;
	}

	stmt_print(s->next, indent);
}