
#include<stdio.h>
#include<stdlib.h>
#include "../include/arbitrary_commands.h"
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>

int execute_arbitrary_commands(int argc, char* argv[]){
    if(argc==0){
        return -1;
    }
    if(argv[0]==NULL){
        return -1;
    }

    pid_t pid=fork();
    if(pid<0){
        printf("FORK failed\n");
        return -1;
    }
    else if(pid==0){
        //child process runs
        if(execvp(argv[0],argv)==-1){
            fprintf(stderr, "Command not found!\n");
            //it failed
            return 0;
        }
        else{
            
            return 1;
        }
    }
    else{
        //parent runs
        pid_t wait_pid=wait(NULL);
    }
    return 1;
}