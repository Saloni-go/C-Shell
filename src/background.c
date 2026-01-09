#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include <string.h>
#include "../include/background.h"
#include<fcntl.h>
#include "../include/activities.h"
//-----------------------------LLM Generated Code Begins--------------------------------

#define MAX_JOBS 1024

static Background_job jobs[MAX_JOBS];  //array of running background jobs
static int job_count=0; //how many jobs are currently active
static int next_job_id=1;   //monotonically increasing job numbers assigned to each new job.



void run_in_background(int argc, char* argv[], char* command){
    pid_t pid=fork();
    if(pid<0){
        perror("fork failed");
        return;
    }
    if(pid==0){
        //child process
        setpgid(0,0);

        // ----------------LLM Generated Code Begins-----------------------------------------
        //redirect stdin to /dev/null to prevent reading from terminal
        int fd=open("/dev/null",O_RDONLY);
        if(fd>=0){
            dup2(fd,STDIN_FILENO);
            close(fd); //so that background process once after execution cant take input from the termnal
        }

        if(execvp(argv[0],argv)==-1){
            perror("execvp failed");
            exit(1);
        }
    }
    else{
        //parent process
        if(job_count<MAX_JOBS){
            jobs[job_count].job_id=next_job_id++;
            jobs[job_count].pid=pid;
            strncpy(jobs[job_count].command, command, sizeof(jobs[job_count].command));
            jobs[job_count].command[sizeof(jobs[job_count].command)-1]='\0';
            job_count++;

            printf("[%d] %d\n", jobs[job_count-1].job_id, pid);

            //add it in activites list also
            add_process(pid, argv[0],1);
        }else{
            fprintf(stderr, "Too many background jobs\n");
        }
    }
}

void check_background_jobs(){
    int status;
    pid_t pid;

    for(int i=0;i<job_count;i++){
        pid=waitpid(jobs[i].pid, &status, WNOHANG); //checks if a specific chil has finished without blocking

        if(pid>0){
            if(WIFEXITED(status)){  //if the job is still running, returns 0 else returns pid and fills status
                printf("%s with pid %d exited normally\n", jobs[i].command, jobs[i].pid);
            }
            else{
                printf("%s with pid %d  exited abnormally\n", jobs[i].command, jobs[i].pid);
            }

            //remove background process from activites tracking
            remove_process(pid);

            //shift remaining jobs
            for(int j=i;j<job_count-1;j++){
                jobs[j]=jobs[j+1];
            }
            job_count--;
            i--;
        }
    }
}

//-----------------------------LLM Generated Code Ends-----------------------------------