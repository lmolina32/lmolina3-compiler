/* scratch.h */

#ifndef SCRATCH_H
#define SCRATCH_H

#include <stdio.h>

/* Macros */

#define     MAX_SCRATCH_REGISTERS 7 

/* Functions */

int         scratch_alloc();
void        scratch_free(int r);
const char *scratch_name(int r);

#endif 