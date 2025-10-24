/* utils.h: Bminor compiler utility function */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Macros */
#define MALLOC_CHECK(ptr) \
    do { \
        if (ptr == NULL){ \
            fprintf(stderr, "Malloc return NULL pointer, cannot continue compiling\n"); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)


/* Malloc */

#define safe_malloc(t, s) ({ \
    void *_ptr = malloc((t) * (s)); \
    MALLOC_CHECK(_ptr); \
    _ptr; \
}) 

#define safe_calloc(t, s) ({ \
    void *_ptr = calloc((s), (t)); \
    MALLOC_CHECK(_ptr); \
    _ptr; \
})

/* strdup */
#define safe_strdup(s) ({ \
    char *_ptr = strdup(s); \
    MALLOC_CHECK(_ptr); \
    _ptr; \
})

/* Miscellaneous */

#define chomp(s)            if (strlen(s)) { s[strlen(s) - 1] = 0; }
#define min(a, b)           ((a) < (b) ? (a) : (b))
#define streq(a, b)         (strcmp(a, b) == 0)

#endif