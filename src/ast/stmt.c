/* stmt.c: stmt structure functions */

#include "bminor_context.h"
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
			expr_print(s->expr, stdout);
			printf(";\n");
			break;
		case STMT_IF_ELSE:
			print_indent(indent);
			printf("if (");
			expr_print(s->expr, stdout);
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
			expr_print(s->init_expr, stdout);
			putchar(';');
			expr_print(s->expr, stdout);
			putchar(';');
			expr_print(s->next_expr, stdout);
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
			expr_print(s->expr, stdout);
			printf(";\n");
			break;
		case STMT_RETURN:
			print_indent(indent);
			printf("return");
			if (s->expr){
				putchar(' ');
				expr_print(s->expr, stdout);
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

	// Case 1: stmt is block -> enter new scope
    if (s->kind == STMT_BLOCK){ 	 
        scope_enter();
		if (s->body) s->body->func_sym = s->func_sym;
        stmt_resolve(s->body);
        scope_exit();
	// Case 2: stmt is for and if else 
    } else if (s->kind == STMT_FOR || s->kind == STMT_IF_ELSE){ 
		// Case 2a: stmt body is decl which is not allowed in single line for or if
		if(s->body && s->body->kind == STMT_DECL){
			fprintf(stderr, "resolver error: '%s' can not be declared in a single-line %s\n", s->body->decl->name, s->kind == STMT_FOR ? "for loop" : "if statement");
			decl_resolve(s->body->decl);
			b_ctx.resolver_errors += 1;
		// Case 2b: stmt is not a STMT_DECL recurse down
		} else {
			if (s->body) s->body->func_sym = s->func_sym;
        	stmt_resolve(s->body);
		}
	// Case 2: stmt if not BLOCK, FOR, or IF_ELSE recurse down
	} else {
		if (s->body) s->body->func_sym = s->func_sym;
        stmt_resolve(s->body);
    }

	// Case 3: else body 
	if (s->else_body){
		// Case 3a: If body is decl throw error, single line decls aren't allowed
		if (s->else_body->kind == STMT_DECL) {  
			fprintf(stderr, "resolver error: '%s' can not be declared in a single-line %s\n", s->else_body->decl->name, "else statement");
			decl_resolve(s->body->decl);
			b_ctx.resolver_errors += 1;
		// Case 3b: else boyd is not decl, recurse down and enter new scope
		} else{
			s->else_body->func_sym = s->func_sym;
			stmt_resolve(s->else_body);
		}
	}

	if (s->next) s->next->func_sym = s->func_sym;
    stmt_resolve(s->next);
}

/**
 * Perform typechecking on the stmt struct ensuring compatibility for all stmts
 * @param 	s 		ptr to stmt struct to typecheck  
 */
bool stmt_typecheck(Stmt *s){
	if (!s) return false;
	Type *t;
	bool res = false;
	Expr *e = NULL;
	switch(s->kind){
		case STMT_DECL:
			decl_typecheck(s->decl);
			break;
		case STMT_EXPR:
			t = expr_typecheck(s->expr);
			type_destroy(t);
			break;
		case STMT_IF_ELSE:
			t = expr_typecheck(s->expr);
			if (t->kind != TYPE_BOOLEAN || !t) {
				fprintf(stderr, "typechecker error: Condition in 'if' statement must be of type boolean, but got");
				type_print(t, stderr);
				fprintf(stderr, ".\n");
				b_ctx.typechecker_errors++;
			}
			type_destroy(t);
			res = stmt_typecheck(s->body) && stmt_typecheck(s->else_body);
			break;
		case STMT_FOR:
			t = expr_typecheck(s->init_expr);
			type_destroy(t);
			t = expr_typecheck(s->next_expr);
			type_destroy(t);
			t = expr_typecheck(s->expr);
			if (t && t->kind != TYPE_BOOLEAN) {
				fprintf(stderr, "typechecker error: Condition in 'for' loop must be of type boolean, but got");
				type_print(t, stderr);
				fprintf(stderr, ".\n");
				b_ctx.typechecker_errors++;
			}
			type_destroy(t);
			res = stmt_typecheck(s->body);
			break;
		case STMT_PRINT:
			e = s->expr;
			while (e){
				t = expr_typecheck(e->left);
				if (t && (t->kind == TYPE_VOID || t->kind == TYPE_FUNCTION || t->kind == TYPE_AUTO)){
					fprintf(stderr, "Typechecker error: Cannot print type (");
					type_print(t, stderr);
					fprintf(stderr, ")\n");
					b_ctx.typechecker_errors++;
				}
				type_destroy(t);
				e = e->right;
			}
			break;
		case STMT_RETURN:
			t = expr_typecheck(s->expr);
			if (!t) t = type_create(TYPE_VOID, 0, 0, 0);
			Type *func_return_type = s->func_sym->type->subtype;
			// Case 1: return type not set
			if (func_return_type->kind == TYPE_AUTO){
				// Case 1a: empty return (return;) set return type to void 
				if (!t){
					func_return_type->kind = TYPE_VOID;
					fprintf(stdout, "typechecker resolved: return type for function '%s' set to ( void )\n", s->func_sym->name);
				// Case 1b: retuning non valid return type 
				} else if (!type_valid_return(t) || t->kind == TYPE_AUTO){
					fprintf(stderr, "typechecker error: Invalid return type got (");
					type_print(t, stderr);
					fprintf(stderr, " ) but expected either (integer, double, string, char, boolean, or nothing)\n");
					b_ctx.typechecker_errors++;
				// Case 1c: return type valid set return type
				} else {
					func_return_type->kind = t->kind;	
					fprintf(stdout, "typechecker resolved: return type for function '%s' set to (", s->func_sym->name);
					type_print(t, stdout);
					fprintf(stdout, " )\n");
				}
			// Case 2: return type set, check if equals
			} else if (t->kind != func_return_type->kind){
				fprintf(stderr, "typechecker error: Return type mismatch. Expected (");
				type_print(s->func_sym->type->subtype, stderr);
				fprintf(stderr, " ), but got (");
				type_print(t, stderr);
				fprintf(stderr, " ).\n");
				b_ctx.typechecker_errors++;
			}
			type_destroy(t);
			res = true;
			break;
		case STMT_BLOCK:
			res = stmt_typecheck(s->body);
			break;
	}
	return stmt_typecheck(s->next) || res;
}