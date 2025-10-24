/* bminor.c: compiler for the bminor language */

#include "bminor_functions.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

/* Main Execution */

int main(int argc, char *argv[]){
    int argind = 1;
    if (argc < 3){
        fprintf(stderr, "Failed not enough command line arguments\n");
        return EXIT_FAILURE;
    }

    char *command = argv[argind++];
    if (streq(command, "--encode")){
        if (!encode(argv[argind++])) return EXIT_FAILURE;
    } else if (streq(command, "--scan")) {
        if (!scan(argv[argind++])) return EXIT_FAILURE;
    } else if (streq(command, "--parse")){
        if (!parse(argv[argind++])) return EXIT_FAILURE;
    } else { return EXIT_FAILURE; }

    return EXIT_SUCCESS;
}

