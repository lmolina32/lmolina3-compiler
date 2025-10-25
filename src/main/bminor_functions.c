/* bminor_functions.c: functions for main stages of compiler */

#include "bminor_functions.h"
#include "encoder.h"
#include "tokens_to_string.h"
#include "token.h"
#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"
#include "utils.h"

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

    FILE *f = safe_fopen(file_name, "r");

    if(!fgets(encoded_string, BUFSIZ, f)){
        fprintf(stderr, "Read error or Empty file\n");
        fclose(f);
        return false;
    }

    // decode and encode strings
    if (!string_decode(encoded_string, decoded_string)) {
        fprintf(stderr, "Failed to decode encoded string: %s\n", encoded_string);
        fclose(f);
        return false;
    }

    printf("Decoded string: %s\n", decoded_string);
    string_encode(decoded_string, encoded_string);
    printf("\n\nEncoded string from decoded: %s\n", encoded_string);
    
    fclose(f);

    return true;
}

/**
 * Reads in file and scans the code and tokenizes it
 * @param   file_name       name of file to open
 * @return  True if able to scan and tokenize, otherwise false 
 **/
int scan(char *file_name){
    int exit_code = true;
    size_t t;
    yyin = safe_fopen(file_name, "r");

    yyrestart(yyin);
    
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
    yylex_destroy();
    return exit_code;
}

/**
 * Reads in file and parses the file to see if it fits in the 
 * grammar for Bminor  
 * @param   file_name       name of file to open
 * @return  True if able to scan & parse, otherwise false 
 **/
int parse(char *file_name){
    int exit_code = true;
    yyin = safe_fopen(file_name, "r");

    yyrestart(yyin);

    if(yyparse() == 0){
        printf("Prase Successful\n");
    } else {
        fprintf(stderr, "Parse Error\n");
        exit_code = false;
    }

    decl_destroy(root);
    fclose(yyin);
    yylex_destroy();
    return exit_code;
}

/**
 * Reads in File, parses File then pretty prints out the program
 * @param   file_name       name of file to open 
 * @return  True if valid parse and able to pretty print, otherwise false 
 */
int pretty_print(char *file_name){
    int exit_code = true;
    yyin = safe_fopen(file_name, "r");

    yyrestart(yyin);

    if(yyparse() == 0){
        decl_print(root, 0);
    } else {
        fprintf(stderr, "Parse Error\n");
        exit_code = false;
    }

    decl_destroy(root);
    fclose(yyin);
    yylex_destroy();
    return exit_code;
}