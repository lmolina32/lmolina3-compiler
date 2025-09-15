/* bminor_functions.c: functions for main stages of compiler */

#include "bminor_functions.h"
#include "encoder.h"
#include "tokens.h"

#include <stdbool.h>
#include <string.h>
#include <errno.h>

/* functions */

/**
 * Reads in file containing a string literal, then decodes and encodes the string
 * @param   s       name of file to open
 * @return  True if valid string literal, otherwise false 
 **/
int encode(char *file_name){
    char encoded_string[BUFSIZ];
    char decoded_string[BUFSIZ];

    FILE *f = fopen(file_name, "r");

    if (!f) {
        fprintf(stderr, "%s %s\n", strerror(errno), file_name);
        return false;
    }

    if(!fgets(encoded_string, BUFSIZ, f)){
        fprintf(stderr, "Read error or Empty file\n");
        fclose(f);
        return false;
    }

    // decode and encode strings
    if (!string_decode(encoded_string, decoded_string)) {
        fclose(f);
        return false;
    }

    printf("decoded string: %s\n", decoded_string);
    string_encode(decoded_string, encoded_string);
    printf("encoded string from decoded: %s\n", encoded_string);
    
    fclose(f);

    return true;
}

/**
 * Reads in file with line of code, scans the code and tokenizes it
 * @param   file_name       name of file to open
 * @return  True if able to scan and tokenize, otherwise false 
 **/
int scan(char *file_name){
    yyin = fopen(file_name, "r");

    if (!yyin) {
        fprintf(stderr, "%s %s\n", strerror(errno), file_name);
        return false;
    }
    while (1) {
        token_t t = yylex();
        if (t == TOKEN_EOF) break;
        printf("token: %d  text: %s\n", t, yytext);
    }

    
    fclose(yyin);
    return true;
}