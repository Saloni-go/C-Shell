#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/wait.h>
#include "../include/signals.h"

pid_t foreground_pgid=-1;
Job jobs[100];
int job_count=0;

//signit: CTRL+c, kills foreground (not shell)
void sigint_handler(int sig){
    if(foreground_pgid>0){
        killpg(foreground_pgid, SIGINT);
    }
}

//sigtstp: CTRL+Z, stops foreground and adds to the job list
void sigtstp_handler(int sig){
    if(foreground_pgid>0){
        killpg(foreground_pgid, SIGTSTP);

        jobs[job_count].job_id=job_count+1;
        jobs[job_count].pgid=foreground_pgid;
        strcpy(jobs[job_count].command, "some_command");
        strcpy(jobs[job_count].status,"Stopped");

        printf("[%d] Stopped %s\n", jobs[job_count].job_id, jobs[job_count].command);
        job_count++;
    }
}

//CTRL+D (EOF): Exists shell cleanly
void handle_eof(){
    printf("logout\n");
    //kill all the jobs before exit
    for(int i=0;i<job_count;i++){
        killpg(jobs[i].pgid,SIGKILL);
    }
    exit(0);
}

