/*
This is the standard library of functions for bminor, implemented in C.  
The print statement in bminor expects there to exist a function
for each type that can be printed.  So, the following bminor code:

x: int = 10;
b: boolean = true;
x: string = "hello";

print x, b, s;

Is effectively translated to the following C code:

print_integer(x);
print_boolean(b);
print_string(s);

And the following bminor code:

x = a ^ b;

Is effectively this C code:

x = integer_power(a,b);
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void print_integer(long x){
	printf("%ld", x);
}

void print_string(const char *s){
	printf("%s", s);
}

void print_boolean(int b){
	printf("%s", b ? "true" : "false");
}

void print_character(char c){
	printf("%c", c);
}

void print_carray(long arr[]){
	printf("%p", (void *)arr);
}

void print_array_str(long arr[]){
	printf("array: {\"%s\"", (char *)arr[1]);
	for (long i = 2; i < arr[0] + 1; i++){
		printf(", \"%s\"", (char *)arr[i]);
	}
	printf("} ");
}

void print_array_int(long arr[]){
	printf("array: {%ld", arr[1]);
	for (long i = 2; i < arr[0] + 1; i++){
		printf(", %ld", arr[i]);
	}
	printf("} ");
}

void print_array_bool(long arr[]){
	printf("array: {%s", arr[1] ? "true" : "false");
	for (long i = 2; i < arr[0] + 1; i++){
		printf(", %s", arr[i] ? "true" : "false");
	}
	printf("} ");
}

void print_array_char(long arr[]){
	printf("array: {'%c'", (char)arr[1]);
	for (long i = 2; i < arr[0] + 1; i++){
		printf(", '%c'", (char)arr[i]);
	}
	printf("} ");
}

void check_bounds(long arr[], long index){
	if (index < 0 || arr[0] <= index){
		fprintf(stderr,"Index out of bounds\n");
		exit(EXIT_FAILURE);
	}
}

long str_equal(char *s1, char *s2){
	return strcmp(s1, s2) == 0;
}

long str_not_equal(char *s1, char *s2){
	return strcmp(s1, s2) != 0;
}

long integer_power(long x, long y){
	long result = 1;
	while(y > 0) {
		result = result * x;
		y = y - 1;
	}
	return result;
}
