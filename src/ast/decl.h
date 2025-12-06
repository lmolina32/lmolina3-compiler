/* decl.h: decl structure and functions */

#ifndef DECL_H
#define DECL_H

#include <stdio.h>

/* Forward declaration */

typedef struct Type Type;
typedef struct Expr Expr;
typedef struct Stmt Stmt;
typedef struct Symbol Symbol;

/* Macros */
#define MAX_INT_ARGS 6 
#define MAX_DOUBLE_ARGS 8

/* Structure */

extern const char *int_args[MAX_INT_ARGS]; 
extern const char *double_args[MAX_DOUBLE_ARGS]; 

typedef struct Decl Decl;

struct Decl {
	char *name;			// name of declaration 
	Type *type;			// data type of decl 
	Expr *value;		// associated value of decl
	Stmt *code; 		// code associated with decl (funcs)
	Symbol *symbol;     // include constants, vars, and funcs 
	Decl *next;			// next decl (ptr)
	int owner;			// flag for whose owner of the symbol 
	int local;			// count of local params 
};

/* Functions */

Decl    *decl_create(const char *name, Type *type, Expr *value, Stmt *code, Decl *next);
void	 decl_destroy(Decl *d);
void 	 decl_print(Decl *d, int indent);
Decl	*decl_copy(Decl *d);
void     decl_resolve(Decl *d);
void 	 decl_typecheck(Decl *d);
void 	 decl_codegen(Decl *d, FILE *f);

#endif


