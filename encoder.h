/* encoder.h*/

#pragma once 

#include <stdio.h>

/* Functions */

int string_decode( const char *es, char *s );
void string_encode( const char *s, char *es );
int char_to_hex(char c);