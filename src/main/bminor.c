/* bminor.c: compiler for the bminor language */

#include "bminor_functions.h"
#include "utils.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Main Execution */

int main(int argc, const char *argv[]){
    int argind = 1;
    bool status = true;

    // error check for correct arguments 
    if (argc > 1 && (streq(argv[1], "-h") || streq(argv[1], "--help"))) {
        usage(argv[0]);
        return EXIT_SUCCESS;
    }

    if (argc < 3){
        fprintf(stderr, "Failed not enough command line arguments\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    
    if (streq(argv[1], "--codegen") && argc != 4){
        fprintf(stderr, "Failed not enough command line arguments\n");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *command = argv[argind++];
    const char *filename = argv[argind++];
    const char *output_file = NULL;

    // parse commands
    if (streq(command, "--encode")){
        status = encode(filename);
    } else if (streq(command, "--scan")) {
        status = scan(filename);
    } else if (streq(command, "--parse")){
        status = parse(filename);
    } else if (streq(command, "--print")){
        status = pretty_print(filename);
    } else if (streq(command, "--resolve")){
        status = resolve(filename, true);
    } else if (streq(command, "--typecheck")){
        status = typecheck(filename, true);
    } else if (streq(command, "--codegen")){
        output_file = argv[argind++]; 
        status = codegen(filename, output_file);
    }else { 
        fprintf(stderr, "Failed: Unknown command '%s'\n", command);
        usage(argv[0]);
    }

    return status ? EXIT_SUCCESS : EXIT_FAILURE;
}

