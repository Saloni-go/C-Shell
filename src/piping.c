#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

//child process->reader
//parent process->writer

int pipeline(int argc, char* argv[]){
    int pipefd[2]; //fd[0]=read, fd[1]=write

    

    pid_t process_pids[1024]; //assume a maximum of 1024 processes in the pipeline, store pid of all of them
    int prev_input=STDIN_FILENO; //stored to see if we are at starting or middle of the piping commands.. basically to see if we need input from previous command or not
   
    int count=0; //number of commands in the pipeline
    int start=0;  //wehre the current command begins in argv
    int has_input_redirect=0;
    int has_output_redirect=0;

    for(int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "<") == 0) has_input_redirect = 1;
        if (strcmp(argv[i], ">") == 0 || strcmp(argv[i], ">>") == 0) has_output_redirect = 1;
    }
    

    for(int i=0;i<argc || (i==argc && start<argc); i++){
        if((i<argc && strcmp(argv[i],"|")==0) || i==argc){  //either | or end of input -> command ends, so basically eg ls -l | grep text. First time | comes means its end of the command ls - l and next time i ends means its end of second command grep
            char* args[64];
            int arg_count=0;

            for(int j=start; j<i;j++){  //from start of that command start and go till end and store it
                args[arg_count]=argv[j];
                arg_count++;
            }
            args[arg_count]=NULL; //execpt requires a null termination at the end of the commands

            if(arg_count==0){  //skipd empty commands like ls | | cat
                start=i+1;
                continue; 
            }

            // if something isnt the last command, we create a pipe and then fork a process
            if(i<argc){
                int x=pipe(pipefd);
                if(x==-1){
                    perror("pipe failed\n");
                    exit(1);
                }
            }

            process_pids[count]=fork();  //stores the pid of child process at each index for each command
            if(process_pids[count]<0){
                perror("fork failed");
                exit(1);
            }

            else if(process_pids[count]==0){
                //child process
                //if its not the first command, replace it with the previous input
                if(prev_input !=STDIN_FILENO){
                    dup2(prev_input, STDIN_FILENO);
                    close(prev_input);
                }
                
                if(i<argc){ //if its not the last command, replace stdout with pipe write end
                    dup2(pipefd[1],STDOUT_FILENO);
                    close(pipefd[0]);
                    close(pipefd[1]);
                }

                 // Only the first command handles input redirection.
                 if (count == 0 && has_input_redirect) {
                    if (input_redirection(&arg_count, args) != 0) {
                        exit(1);
                    }
                }
                
                // Only the last command handles output redirection.
                if (i == argc && has_output_redirect) {
                    if (output_redirection(&arg_count, args) != 0) {
                        exit(1);
                    }
                }
                
                execvp(args[0], args);
                perror("execvp failed\n");
                exit(1);
            }
            else{
                //parent process
                if(prev_input!=STDIN_FILENO){
                    close(prev_input);  //closes previous pipes which are not required now
                    // prev_input=pipefd[0];
                }

                if(i<argc){
                    close(pipefd[1]);
                    prev_input=pipefd[0];
                }
                count++;
            }

            start=i+1;
        }
    }

    //parent waits for all children
    for(int i=0;i<count;i++){
        waitpid(process_pids[i],NULL,0);
    }           
            
        
    

  
    return 0;
}
