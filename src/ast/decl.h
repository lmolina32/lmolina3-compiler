/* decl.h: decl structure and functions */

#ifndef DECL_H
#define DECL_H

#include <stdio.h>

/* Forward declaration */

typedef struct Type Type;
typedef struct Expr Expr;
typedef struct Stmt Stmt;
typedef struct Symbol Symbol;

/* Structure */

typedef struct Decl Decl;

struct Decl {
	char *name;			// name of declaration 
	Type *type;			// data type of decl 
	Expr *value;		// associated value of decl
	Stmt *code; 		// code associated with decl (funcs)
	Symbol *symbol;     // include constants, vars, and funcs 
	Decl *next;			// next decl (ptr)
};

/* Functions */

Decl*	 decl_create(const char *name, Type *type, Expr *value, Stmt *code, Decl *next);
void	 decl_destroy(Decl *d);
void 	 decl_print(Decl *d, int indent);
Decl*	 decl_copy(Decl *d);
void     decl_resolve(Decl *d);

#endif


