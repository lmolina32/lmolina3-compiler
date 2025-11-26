/* scratch.c: function for scratch registers */

#include "scratch.h"

#include <stdio.h>
#include <stdlib.h>

/* Globals */

// 7 scratch registers -> {rbx, r10, r11, r12, r13, r14, r15}
// 0 -> not in use 
// 1 -> in use 
int scratch_registers[MAX_SCRATCH_REGISTERS] = {0};

static const char *register_names[MAX_SCRATCH_REGISTERS] = {
    "%%rbx",
    "%%r10",
    "%%r11",
    "%%r12",
    "%%r13",
    "%%r14",
    "%%r15"
};

/* Functions */

/**
 * This function looks for open register from the scratch registers and returns its number 
 * @return  integer corresponding to scratch register 
 */
int scratch_alloc(){
    for (int i = 0; i < MAX_SCRATCH_REGISTERS; i++){
        if (!scratch_registers[i]){
            scratch_registers[i] = 1;
            return i;
        }
    } 

    fprintf(stderr, "scratch_alloc: Ran out of scratch registers\n");
    exit(EXIT_FAILURE);
}

/**
 * This function takes scratch register numbers and frees it (e.g sets it as usable again)
 * @return   r      integer corresponding to scratch register to free
 */
void scratch_free(int r){
    if (r < 0 || r >= MAX_SCRATCH_REGISTERS){
        fprintf(stderr, "scratch_free: Invalid scratch register number passed, scratch registers range from 0-6\n");
        return;
    }
    scratch_registers[r] = 0;
}

/**
 * This function takes scratch register number and returns the name
 * @return   static string that contains the register name 
 */
const char * scratch_name(int r){
    if (r < 0 || r >= MAX_SCRATCH_REGISTERS){
        fprintf(stderr, "scratch_name: Invalid scratch register number passed, scratch registers range from 0-6\n");
        return NULL;
    }
    return register_names[r];
}