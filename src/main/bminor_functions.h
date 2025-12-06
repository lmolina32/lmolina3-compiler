/* bminor_functions.h */

#ifndef BMINOR_FUNCTIONS_H
#define BMINOR_FUNCTIONS_H

#include <stdio.h>
#include <stdbool.h>

/* Functions */

void     usage(const char *program);
bool     encode(const char *file_name);
bool     scan(const char *file_name);
bool     parse(const char *file_name);
bool     pretty_print(const char *file_name);
bool     resolve(const char *file_name, bool cleanup);
bool     typecheck(const char *file_name, bool cleanup);
bool     codegen(const char *file_name, const char *file_output);

#endif 
