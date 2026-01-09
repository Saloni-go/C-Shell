#include <stdio.h>
#include<string.h>
#include "../include/input.h"

int read_input(char* input, size_t size){
    // scanf("%s", input);
    // Use fgets to take input of the whole line

    if(fgets(input, size, stdin)==NULL){
        return 0;
    }

    input[strcspn(input, "\n")]='\0'; //returns the length of the initial segment of S which consists of characters before the dirst occurence of reject here \n, returns the number of characters in the first string before the 1st occurence
    //replaces the index of \n with \0 i.e. EOF.
    return 1;
}