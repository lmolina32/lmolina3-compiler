/* label.h */

#ifndef LABEL_H
#define LABEL_H

#include <stdio.h>

/* Globals */
#define     MAX_NAME 1<<8

/* Functions */

int         label_create();
const char *label_name(int label);

#endif 