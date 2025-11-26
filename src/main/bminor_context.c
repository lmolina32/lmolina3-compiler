/* bminor_context.c: source file to init context */

#include "bminor_context.h"

#include <stdio.h>

Context b_ctx = {
    .resolver_errors = 0,
    .typechecker_errors = 0,
    .codegen_errors = 0,
};
