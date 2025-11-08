/* bminor_context.h: Error context for the compiler */

#ifndef BMINOR_CONTEXT
#define BMINOR_CONTEXT

#include <stdio.h>

typedef struct Context Context;

struct Context {
    int resolver_errors;
    int typechecker_errors;
};

extern Context b_ctx;

#endif