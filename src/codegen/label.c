/* label.h: create labels for codegen */

#include "label.h"

#include <stdio.h>
#include <stdlib.h>

/* Globals */
static int label_count = 0;

/* Functions */

/**
 * Increates label global count and returns the number 
 * @return  integer corresponding to the current label number 
 */
int label_create(){
    return label_count++;
}

/**
 * Takes in label number, then creates and returns the name of the label 
 * @param   label       Integer for the specified label to create 
 * @return  static string corresponding to the label created 
 */
const char *label_name(int label){
    static char name[MAX_NAME] = {0};
    sprintf(name, ".L%d", label);
    return name;
}