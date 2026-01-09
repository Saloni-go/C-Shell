#ifndef BACKGROUND_H
#define BACKGROUND_H
#include<sys/wait.h>

typedef struct{
    int job_id;
    pid_t pid;
    char command[256];
}Background_job;

void run_in_background(int argc, char* argv[], char* command);
void check_background_jobs();


#endif