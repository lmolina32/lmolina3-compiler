/* stmt.c: stmt structure functions */

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
	s->decl = NULL;
	expr_destroy(s->init_expr);
	s->init_expr = NULL;
	expr_destroy(s->expr);
	s->expr = NULL;
	expr_destroy(s->next_expr);
	s->next_expr = NULL;
	stmt_destroy(s->body);
	s->body = NULL;
	stmt_destroy(s->else_body);
	s->else_body = NULL;
	Stmt *next = s->next;
	s->next = NULL;
	free(s);

	stmt_destroy(next);
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
			printf(") ");
			
			if (s->body && s->body->kind == STMT_BLOCK){
				printf("{\n");
				stmt_print(s->body->body, indent + 1);
				print_indent(indent);
				putchar('}');
			} else {
				printf("{\n");
				stmt_print(s->body, indent + 1);
				print_indent(indent);
				putchar('}');
			}
			
			if (s->else_body){
				printf(" else ");
				if (s->else_body->kind == STMT_BLOCK){
					printf("{\n");
					stmt_print(s->else_body->body, indent + 1);
					print_indent(indent);
					printf("}\n");
				} else {
					printf("{\n");
					stmt_print(s->else_body, indent + 1);
					print_indent(indent);
					printf("}\n");
				}
			} else {
				printf("\n");
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
			printf(")");
			
			if (s->body && s->body->kind == STMT_BLOCK){
				printf(" {\n");
				stmt_print(s->body->body, indent + 1);
				print_indent(indent);
				printf("}\n");
			} else {
				printf(" {\n");
				stmt_print(s->body, indent + 1);
				print_indent(indent);
				printf("}\n");
			}
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
				putchar(' ');
				expr_print(s->expr);
			}
			printf(";\n");
			break;
		case STMT_BLOCK:
			print_indent(indent);
			printf("{\n");
			stmt_print(s->body, indent + 1);
			print_indent(indent);
			printf("}\n");
			break;
	}

	stmt_print(s->next, indent);
}

/**
 * Perform a deep copy on the stmt structure 
 * @param 	s 		Statement stucture to create a deep copy of
 * @return  ptr to deep copy of structure, otherwise NULL
 */
Stmt *stmt_copy(Stmt *s){
	if (!s) return NULL;
	return stmt_create(s->kind, decl_copy(s->decl), expr_copy(s->init_expr), expr_copy(s->expr), expr_copy(s->next_expr), stmt_copy(s->body), stmt_copy(s->else_body), stmt_copy(s->next));

}

/**
 * Perform name resolution for statement structures 
 * @param   s       Statement structure to perform name resolution 
 **/
void stmt_resolve(Stmt *s){
    if (!s) return;
    decl_resolve(s->decl);
    expr_resolve(s->init_expr);
    expr_resolve(s->expr);
    expr_resolve(s->next_expr);

    if (s->kind == STMT_BLOCK){ 	// new block enter scope 
        scope_enter();
        stmt_resolve(s->body);
        scope_exit();
    } else if (s->kind == STMT_FOR || s->kind == STMT_IF_ELSE){  // check single decl in for and if stmts
		if(s->body && s->body->kind == STMT_DECL){
			fprintf(stderr, "resolver error: '%s' can not be declared in a single-line %s\n", s->body->decl->name, s->kind == STMT_FOR ? "for loop" : "if statement");
			decl_resolve(s->body->decl);
			stack.status = 1;
		} else {
        	stmt_resolve(s->body);
		}
	} else {
        stmt_resolve(s->body);
    }

	if (s->else_body){
		if (s->else_body->kind == STMT_DECL) {  // check decls in single else stmts
			fprintf(stderr, "resolver error: '%s' can not be declared in a single-line %s\n", s->else_body->decl->name, "else statement");
			decl_resolve(s->body->decl);
			stack.status = 1;
		} else{
			scope_enter();
			stmt_resolve(s->else_body);
			scope_exit();
		}
	}

    stmt_resolve(s->next);
}
