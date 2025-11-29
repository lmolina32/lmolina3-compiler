/* bminor_context.h: Error context for the compiler */

#ifndef BMINOR_CONTEXT
#define BMINOR_CONTEXT

#include <stdio.h>
#include <stdbool.h>

typedef struct Context Context;

struct Context {
    int resolver_errors;
    int typechecker_errors;
    int codegen_errors;
    bool data_flag;
    bool text_flag;
};

extern Context b_ctx;

#endif