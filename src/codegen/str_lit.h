/* str_lit.h: linked list for string literals */

#ifndef STR_LIT_H
#define STR_LIT_H

#include <stdio.h>

/* Forward Declaration */

typedef struct Symbol Symbol;

/* Structure */

typedef struct String_lit String_lit;

struct String_lit {
    char *literal;
    char *label;
    Symbol *sym;
    String_lit *next;    
};

typedef struct String_head String_head;

struct String_head {
    String_lit *head;
    String_lit *tail;
};

/* Globals */

extern String_head string_ll; 

/* Functions */

String_lit  *string_alloc(Symbol *s, const char *literal, const char *label);
void         string_lit_destroy();
void         string_print(FILE *f);

#endif 