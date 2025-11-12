/* bminor_functions.c: functions for main stages of compiler */

#include "bminor_functions.h"
#include "bminor_context.h"
#include "encoder.h"
#include "tokens_to_string.h"
#include "token.h"
#include "decl.h"
#include "expr.h"
#include "param_list.h"
#include "stmt.h"
#include "symbol.h"
#include "type.h"
#include "scope.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

/* Globals */

extern FILE   *yyin;
extern int     yylex();
extern char   *yytext; 
extern int     yyparse();
extern int     yyrestart();
extern int     yylex_destroy();
extern Decl    *root;

/* Helper Functions */

/**
 * Handles common setup: opens the file and sets up the scanner.
 * @param file_name name of file to open
 * @return True on successful setup, otherwise false.
 */
static bool setup_compiler(const char *file_name) {
    if (!file_name) {
        fprintf(stderr, "Error: Filename is NULL.\n");
        return false;
    }

    yyin = safe_fopen(file_name, "r");
    if (!yyin) return false; 
    yyrestart(yyin);
    return true;
}

/**
 * Handles common cleanup: destroys AST, closes file, and destroys scanner state.
 * @param destroy_ast True if the root AST node (root) should be destroyed.
 */
static void cleanup_compiler(bool destroy_ast) {
    if (destroy_ast && root) {
        decl_destroy(root);
        root = NULL; 
    }

    if (yyin) {
        fclose(yyin);
        yyin = NULL;
    }
    yylex_destroy();
}

/* functions */

/**
 * Display usage message.
 * @param       program     String containing name of program.
 **/
void usage(const char *program) {
    // Standard usage format: program [stage] [input file]
    fprintf(stderr, "Usage: %s [options] <Bminor source file>\n\n", program); 
    fprintf(stderr, "Options (Choose one stage):\n");
    fprintf(stderr, "   --encode       Reads a file containing a string literal, decodes and re-encodes it.\n");
    fprintf(stderr, "   --scan         Scans the source file and prints a list of tokens.\n");
    fprintf(stderr, "   --parse         Parses the source file against the Bminor grammar (Syntax Check).\n");
    fprintf(stderr, "   --print         Parses the file and pretty-prints the resulting AST.\n");
    fprintf(stderr, "   --resolve       Performs name resolution (semantic check).\n");
    fprintf(stderr, "   --typecheck     Performs type checking (semantic check).\n");
    fprintf(stderr, "\nGeneral Options:\n");
    fprintf(stderr, "   -h or --help    Print this help message.\n");
}

/**
 * Reads in file containing a string literal, then decodes and encodes the string
 * @param   s       name of file to open
 * @return  True if valid string literal, otherwise false 
 **/
bool encode(const char *file_name){
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
bool scan(const char *file_name){
    if (!setup_compiler(file_name)) return false;
    
    bool exit_code = true;
    size_t t;

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
    
    cleanup_compiler(false);
    return exit_code;
}

/**
 * Reads in file and parses the file to see if it fits in the 
 * grammar for Bminor  
 * @param   file_name       name of file to open
 * @return  True if able to scan & parse, otherwise false 
 **/
bool parse(const char *file_name){
    if (!setup_compiler(file_name)) return false;

    bool exit_code = true;
    if(yyparse() == 0){
        printf("Prase Successful\n");
    } else {
        fprintf(stderr, "Parse Error\n");
        exit_code = false;
    }

    cleanup_compiler(true);
    return exit_code;
}

/**
 * Reads in File, parses File then pretty prints out the program
 * @param   file_name       name of file to open 
 * @return  True if valid parse and able to pretty print, otherwise false 
 */
bool pretty_print(const char *file_name){
    if (!setup_compiler(file_name)) return false;

    bool exit_code = true;
    if(yyparse() == 0){
        decl_print(root, 0);
    } else {
        fprintf(stderr, "Parse Error\n");
        exit_code = false;
    }

    cleanup_compiler(true);
    return exit_code;
}

/**
 * Reads in file, parses File then does name resolution for all decls, stmts, and exprs 
 * @param   file_name       name of file to open 
 * @param   cleanup         cleanup after calling resolve
 * @return  True if valid parse and able to resolve,  otherwise false 
 **/
bool resolve(const char *file_name, bool cleanup){
    if (!setup_compiler(file_name)) return false;

    bool exit_code = true;
    if(yyparse() == 0){
        scope_enter(); 
        decl_resolve(root);
        scope_exit();
        exit_code = b_ctx.resolver_errors != 0 ? false : true;
    } else {
        fprintf(stderr, "Parse Error\n");
        exit_code = false;
    }

    cleanup_compiler(cleanup);
    return exit_code;
}

/**
 * Resolves program and computes typechecking for each expression ensuring compatibility 
 * @param   file_name       Bminor source file to typecheck
 * @return  true if valid types for each expression, otherwise false
 */
bool typecheck(const char *file_name){
    bool exit_code = true;
    if (resolve(file_name, false)){
        decl_typecheck(root);
        exit_code = b_ctx.typechecker_errors != 0 ? false : true;
    } else {
        fprintf(stderr, "Resolver Error\n");
        exit_code = false;
    }

    cleanup_compiler(true);
    return exit_code;
}

