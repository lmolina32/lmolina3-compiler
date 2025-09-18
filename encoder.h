/* encoder.h*/

#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>

/* Constants */

#define MAX_STR_LEN 255

/* Functions */

int     string_decode( const char *es, char *s );
void    string_encode( const char *s, char *es );
int     char_to_hex(char c);

#endif 