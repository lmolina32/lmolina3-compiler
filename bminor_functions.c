/* bminor_functions.c: functions for main stages of compiler */

#include "bminor_functions.h"
#include "encoder.h"
#include "tokens_to_string.h"
#include "token.h"

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
 * Reads in file and scans the code and tokenizes it
 * @param   file_name       name of file to open
 * @return  True if able to scan and tokenize, otherwise false 
 **/
int scan(char *file_name){
    yyin = fopen(file_name, "r");
    size_t t;

    if (!yyin) {
        fprintf(stderr, "%s %s\n", strerror(errno), file_name);
        return false;
    }

    int exit_code = true;
    
    while ((t = yylex()) != 0) {
        switch (t){
            case TOKEN_STRING_LITERAL:
            case TOKEN_CHAR_LITERAL:
            case TOKEN_INTEGER_LITERAL:
            case TOKEN_DOUBLE_LITERAL:
            case TOKEN_DOUBLE_SCIENTIFIC_LITERAL:
            case TOKEN_HEXIDECIMAL_LITERAL:
            case TOKEN_BINARY_LITERAL:
            case TOKEN_IDENTIFIER:
                printf("token: %-32s  text: %s\n", token_names[t % 258], yytext);
                break;
            case TOKEN_ERROR:
                printf("scan error: %s is not valid\n", yytext);
                break;
            default:
                printf("token: %-30s\n" , token_names[t % 258]);
                break;
        }

        if (t == TOKEN_ERROR) exit_code = false;
    }
    
    fclose(yyin);
    return exit_code;
}

/**
 * Reads in file and scans the code and tokenizes it
 * @param   file_name       name of file to open
 * @return  True if able to scan & parse, otherwise false 
 **/
int parse(char *file_name){
    yyin = fopen(file_name, "r");

    if (!yyin) {
        fprintf(stderr, "%s %s\n", strerror(errno), file_name);
        return false;
    }

    yyrestart(yyin);

    if(yyparse() == 0){
        printf("Prase Successful\n");
        return true;
    } else {
        fprintf(stderr, "Parse Error\n");
        return false;
    }
}