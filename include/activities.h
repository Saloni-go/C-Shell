#ifndef ACTIVITES_H
#define ACTIVITES_H
#include<sys/types.h>

typedef struct process{
    pid_t pid;
    char command[1024];
    int running;

}process_t; 

void add_process(pid_t pid, const char* command, int running);
void update_process_states();
void remove_process(pid_t pid);
void activities();

#endif 