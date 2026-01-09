//PRE-PROCESSING

//issue in input/output redirection, as cat<input.txt was considered as a single token by my code.
// hence manually adding spaces before and after < symbol so that its considered separate

//---------------------------LLM Generated Code Begins------------------------
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "../include/utils.h"

void preprocess_input(char* input){
    char temp[1024];
    int j = 0;
    for (int i = 0; input[i] != '\0'; i++) {
        // Add spaces around the redirection symbols, | to make them separate tokens
        if (input[i] == '<' || input[i]=='|' || input[i] == '>' || (input[i] == '>' && input[i+1] == '>')) {
            if (j > 0 && temp[j-1] != ' ') {
                temp[j++] = ' ';
            }
            if (input[i] == '>' && input[i+1] == '>') {
                temp[j++] = '>';
                i++; // Skip the next '>'
            }
            temp[j++] = input[i];
            temp[j++] = ' ';
        } else {
            temp[j++] = input[i];
        }
    }
    temp[j] = '\0';
    strcpy(input, temp);
}

//-------------------------LLM Generated Code Ends---------------------------