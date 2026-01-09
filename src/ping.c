#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/wait.h>
#include<errno.h>
#include<ctype.h>
#include "../include/ping.h"

//---------------LLM Generated Code Begins------------------------------------------

void ping_command(int argc, char* argv[]){
    if(argc!=3){
        fprintf(stderr,"Usage: ping <pid> <signal_number> \n");
        return;
    }

    //Signal number should be numeric
    for(int i=0;argv[2][i]!='\0';i++){
        if(!isdigit((unsigned char)argv[2][i])){
            printf("Invalid Syntax!\n");
            return;
        }
    }

    // Check if PID is a valid number
    for(int i = 0; argv[1][i] != '\0'; i++){
        if(!isdigit((unsigned char)argv[1][i])){
            printf("Invalid syntax\n"); // Added check for PID
            return;
        }
    }

    pid_t pid=(pid_t)atoi(argv[1]);
    int sig_num = atoi(argv[2]);
    int actual_signal=sig_num % 32;


    if(kill(pid, actual_signal)==-1){
        if(errno == ESRCH){
            printf("No such process found\n");
        }
        else{
            perror("kill failed");
        }
        return;
    }

    printf("Sent signal %d to process with pid %d\n", sig_num,pid);
}

//------------------LLM Generated Code Ends------------------------------------------