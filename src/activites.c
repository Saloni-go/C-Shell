#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include "../include/activities.h"
// #define _GNU_SOURCE

//-------------------LLM Generated Code Begins---------------------------------------

#define MAX_PROCESS 1024

static process_t process_table[MAX_PROCESS];
static int proc_count=0;

//add a process to the table
void add_process(pid_t pid, const char* command, int running){ //running->running 1 & stopped 0
    if(proc_count<MAX_PROCESS){ // if the table is not yet full
        process_table[proc_count].pid=pid;
        strncpy(process_table[proc_count].command, command, sizeof(process_table[proc_count].command));
        process_table[proc_count].command[sizeof(process_table[proc_count].command)-1]='\0';//ensure that the string is null terminated
        process_table[proc_count].running=running;  //current state
        proc_count++;
    }
}

void remove_process(pid_t pid){
    for(int i=0;i<proc_count;i++){
        if(process_table[i].pid==pid){
            process_table[i]=process_table[proc_count-1]; //after deletion, instead of shifting everythin we just replace the deleted entry with the lst entry.
            proc_count--;
            return; 
        }
    }
}

void update_process_states(){
    int status;
    pid_t pid;
    for(int i=0;i<proc_count;i++){
        pid=waitpid(process_table[i].pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        //WNOHANG: returns immediately (dont block)
        //WUNTRACED: report if process is stopped
        //WCONTINUED: report if process resumed after stop
        if(pid>0){
            if(WIFEXITED(status) || WIFSIGNALED(status)){
                remove_process(pid);
                i--;
            }
            else if(WIFSTOPPED(status)){
                process_table[i].running=0; //stopped
            }
            else if(WIFCONTINUED(status)){
                process_table[i].running=1; //resumed
            }
        }
    }
}


//comparator for qsort
static int cmp_process(const void* a,const void* b){
    const process_t *pa=(const process_t*)a;
    const process_t *pb=(const process_t*)b;
    return strcmp(pa->command, pb->command);
}

//print activites
void activities(){
    update_process_states();
    if(proc_count==0){
        printf("No active processes\n");
        return;
    }
    qsort(process_table, proc_count, sizeof(process_t), cmp_process);
    for(int i=0;i<proc_count;i++){
        printf("[%d] : %s - %s\n", process_table[i].pid, process_table[i].command, process_table[i].running ? "Running" : "Stopped");
    }
}


//------------------------LLM Generated Code Ends----------------------------------