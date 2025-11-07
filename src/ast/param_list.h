/* param_list.h: param_list structure and functions */

#ifndef PARAM_LIST_H
#define PARAM_LIST_H

#include <stdio.h>

/* Forward Declaration */

typedef struct Type Type;
typedef struct Symbol Symbol;

/* Structure */

typedef struct Param_list Param_list; 

struct Param_list {
	char *name;			// identifier of arg
	Type *type;			// data type of arg 
	Symbol *symbol;		// include consts, vars, and funcs 
	Param_list *next;	// ptr to next arg
};

/* Functions */

Param_list	    *param_list_create(const char *name, Type *type, Param_list *next);
void			 param_list_destroy(Param_list *a);
void 			 param_list_print(Param_list *a);
Param_list      *param_list_copy(Param_list *a);
void             param_list_resolve(Param_list *a);

#endif
