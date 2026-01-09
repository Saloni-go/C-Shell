#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include "redirection.h"

//------------------------LLM Generated Code Begins--------------------------------------

int input_redirection(int* argc, char** args){
    int i;
    char* filename=NULL;
    
    // Find the last input redirection symbol
    for(i=0; i < *argc; i++){
        if(strcmp(args[i],"<") == 0){
            if((i+1) < *argc){
                filename=args[i+1];
            } else {
                fprintf(stderr,"Syntax error: no file after '<'\n");
                return -1;
            }
        }
    }

    if(filename){
        int fd=open(filename,O_RDONLY);
        if(fd<0){
            fprintf(stderr, "No such file or directory\n");
            return -1;
        }
        if(dup2(fd,STDIN_FILENO)<0){
            perror("dup2 failed.");
            close(fd);
            return -1;
        }
        close(fd);

        // Remove all input redirection symbols and their filenames
        int new_argc = 0;
        for(i = 0; i < *argc; i++){
            if(strcmp(args[i],"<") == 0){
                if((i+1) < *argc){
                    i++; // Skip the filename
                }
            } else {
                args[new_argc++] = args[i];
            }
        }
        args[new_argc] = NULL;
        *argc = new_argc;
    }
    return 0;
}


//-------------------------LLM Generated Code Ends-------------------------------------------

int output_redirection(int* argc, char** args){
    int i;
    char* filename=NULL;
    int flag_append=0;

    // Find the last output redirection symbol
    for(i=0; i < *argc; i++){
        if((strcmp(args[i],">") == 0) || (strcmp(args[i],">>") == 0)){
            if(strcmp(args[i],">>") == 0){
                flag_append=1;
            }

            if((i+1) < *argc){
                filename=args[i+1];
            } else {
                fprintf(stderr, "Syntax error: no file after redirection symbol\n");
                return -1;
            }
        }
    }

    if(filename){
        int fd;
        if(flag_append){
            fd=open(filename, O_WRONLY|O_CREAT| O_APPEND,0644);
        } else {
            fd=open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }

        if(fd<0){
            perror("failed to open output file");
            return -1;
        }

        if(dup2(fd,STDOUT_FILENO)<0){
            perror("dup2 failed");
            close(fd);
            return -1;
        }
        close(fd);

        // Remove all output redirection symbols and their filenames
        int new_argc = 0;
        for(i = 0; i < *argc; i++){
            if((strcmp(args[i],">") == 0) || (strcmp(args[i],">>") == 0)){
                if((i+1) < *argc){
                    i++; // Skip the filename
                }
            } else {
                args[new_argc++] = args[i];
            }
        }
        args[new_argc] = NULL;
        *argc = new_argc;
    }

    return 0;
}


