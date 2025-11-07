/* scope.c: scope function definitions */

#include "hash_table.h"
#include "scope.h"
#include "symbol.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

/* global variable */ 

Symbol_stack stack = {
    .top = 0,
    .size = 0,
    .status = 0,
};

/* functions */

/**
 * This functions adds a new scope (hash table) to the stack and updates metrics 
 **/
void scope_enter(){
    struct hash_table *h = hash_table_create(0, 0);
    stack.size += 1;
    Symbol_node *curr = stack.top;
    Symbol_node *node = safe_calloc(sizeof(Symbol_node), 1);
    node->hashmap = h;
    node->next = curr;
    if (stack.size > 3) { // globals + params layer + 1st local scope -> all other nested calls add them 
        node->local = curr->local;
    }
    stack.top = node;
}

/**
 * This function deletes the top most scope (hash table) from the stack
 **/
void scope_exit(){
    Symbol_node *curr = stack.top;
    stack.top = curr->next;
    stack.size -= 1;
    if (stack.top && stack.size > 3){ // global + params + 1st layer scope 
        stack.top->local = curr->local;
    }
    hash_table_delete(curr->hashmap);
    free(curr);
}

/**
 * This function returns how many scopes are currently in the stack 
 * @return      the number of scopes in the stack
 **/
int scope_level(){
    return stack.size;
}

/**
 * Adds entry to topmost hash table in the stack, binds name with symbol
 * @param   name        The identifier to bind symbol structure 
 * @param   sym         The symbols structure describing the identifier
 **/
void scope_bind( const char *name, Symbol *sym ){
    if (!name || !sym || !stack.top) return;

    sym->which = stack.top->local;
    stack.top->local++;

    if (!hash_table_insert(stack.top->hashmap, name, (void *)sym)){
        fprintf(stderr, "scope_bind: hash table insert failed for %s\n", name);
        stack.status = 1;
        exit(1);
    }
}

/**
 * Searches the stack of hash tables from top to bottom, looking for first entry that matches name extactly. 
 * @param   name        Identifier we are trying to find 
 * @return  Struct symbol corresponding to identifier, NULL if not found 
 **/
Symbol *scope_lookup( const char *name ){
    if (!name) return NULL;

    Symbol_node *curr = stack.top;
    while (curr){
        Symbol *sym = (Symbol *)hash_table_lookup(curr->hashmap, name);
        if (sym) return sym;
        curr = curr->next;
    }

    return NULL;
}

/**
 * Searches top hash table for first enry that matches name exactly 
 * @param   name        Identifier to search in hash table
 * @return  Struct symbol corresponding to identifier, NULL if not found
 **/
Symbol *scope_lookup_current( const char *name ){
    if (!name || !stack.top) return NULL;
    return (Symbol *)hash_table_lookup(stack.top->hashmap, name);
}

