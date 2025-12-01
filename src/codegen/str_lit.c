/* str_lit.c: string literal functions */

#include "str_lit.h"
#include "symbol.h"
#include "encoder.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

/* Globals */

String_head string_ll = {
    .head = 0,
    .tail = 0,
};

/* Functions */

/**
 * Function allocates string literal node and returns it 
 * @param   lit     string literal 
 * @param   label   label associated with string literal 
 */
String_lit *string_alloc(Symbol *s, const char *literal, const char *label){
    String_lit *node = safe_calloc(sizeof(String_lit), 1);
    node->label = safe_strdup(label);
    
    node->sym = s;
    node->literal = safe_strdup(literal);

    if (!string_ll.head && !string_ll.tail){
        string_ll.head = node;
        string_ll.tail = node;
    } else {
        string_ll.tail->next = node;
        string_ll.tail = node;
    }
    return node;
}

/**
 * Function frees all nodes in the string linked list and labels
 */
void string_lit_destroy(){
    String_lit *node = string_ll.head;
    String_lit *dummy = NULL;

    while (node){
        free(node->label);
        free(node->literal);
        dummy = node;
        node = node->next;
        free(dummy);
    }
}

/**
 * Function prints out strings for .data section in code generation 
 * @param   f   FILE ptr to print out .data section 
 */
void string_print(FILE *f){
    if (!f || !string_ll.head) return;
    char es[BUFSIZ] = {0};
    String_lit *node = string_ll.head;

    fprintf(f, ".data\n");
    while (node){
        string_encode(node->literal ? node->literal : "", es);
        fprintf(f, "%s:\n\t.string %s\n", node->label, es);
        node = node->next;
    }

    string_lit_destroy();
}
