/* bminor.c: compiler for the bminor language */

#include "encoder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

/* Main Execution */

int main(int argc, char *argv[]){
    int argind = 1;
    if (argc != 3 || strcmp(argv[argind], "--encode")){
        fprintf(stderr, "Failed not enough command line arguments\n");
        exit(EXIT_FAILURE);
    }

    if (!strcmp(argv[argind++], "--encode")){
        FILE *f = fopen(argv[argind], "r");
        if (!f) {
            fprintf(stderr, "%s %s\n", strerror(errno), argv[argind]);
            return EXIT_FAILURE;
        }
        argind++;

        char encoded_string[BUFSIZ];
        char decoded_string[BUFSIZ];

        if(!fgets(encoded_string, BUFSIZ, f)){
            fprintf(stderr, "Read error or Empty file\n");
            return EXIT_FAILURE;
        }
        if (!string_decode(encoded_string, decoded_string)) return EXIT_FAILURE;
        printf("decoded string: %s\n", decoded_string);
        string_encode(decoded_string, encoded_string);
        printf("encoded string from decoded: %s\n", encoded_string);
        
        fclose(f);
    }
    return EXIT_SUCCESS;
}

