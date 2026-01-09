#ifndef SIGNALS_H
#define SIGNALS_H


#include<signal.h>

typedef struct Job{
    //struct for background jobs
    int job_id;
    pid_t pgid;  //process group id
    char command[256];
    char status[20];//unning/stopped
}Job;

extern pid_t foreground_pgid;
extern Job jobs[100];
extern int job_count;

void sigint_handler(int sig);  //ctrl+c
void sigtstp_handler(int sig);  //ctrl+z
void handle_eof(); //ctrl+d

#endif