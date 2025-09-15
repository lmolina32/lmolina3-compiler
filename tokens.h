#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>

/* Constants */

typedef enum {
    TOKEN_EOF=0,
    TOKEN_ERROR,
    TOKEN_IDENTIFIER,
} token_t;

#endif