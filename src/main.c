#include<stdio.h>
#include<stdlib.h>
#include<pwd.h> //defines the struct passwd, strcture, etc.
#include<unistd.h> //include function declaration, constants, types,etc. which talk directly to the kernel. eg. fork, exec, getuid here.
#include<limits.h> //limits like HOST_NAME_MAX, etc.
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>

#include "../include/parser.h"
#include "../include/prompt.h"
#include "../include/input.h"
#include "../include/hop.h"
#include "../include/reveal.h"
#include "../include/arbitrary_commands.h"
#include "../include/log.h"
#include "../include/redirection.h"
#include "../include/utils.h"
#include "../include/piping.h"
#include "../include/sequential.h"
#include "../include/background.h"
#include "../include/activities.h"
#include "../include/ping.h"
#include "../include/signals.h"

char prev_dir[1024]="";

void sigchild_handler(int sig){
    check_background_jobs();
}

int main(){
    signal(SIGCHLD, sigchild_handler);  
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);

    //In the starting of shell itself, store your base directory
    char base_dir_name[256];
    getcwd(base_dir_name, sizeof(base_dir_name));
    // printf("%s\n", base_dir_name);


    char input[1024];
    char *args[64];

    //Initialize log system, save to $HOME by default
    log_init(NULL);

    while(1){

        //we need to check for any background job before printing the prompt
        check_background_jobs();
        
        printf("%s", build_prompt(base_dir_name));
        fflush(stdout);

        if(!read_input(input, sizeof(input))){
            handle_eof();
        }

        preprocess_input(input); //put spaces just before and after < so that it is not considered as a single token in cat<txt. and can be separated on <.
        //Make a copy of input before strtok destroys it
        char input_copy[1024];
        strncpy(input_copy, input, sizeof(input_copy));
        input_copy[sizeof(input_copy)-1]='\0';

//-------------------------LLM Generated Code Begins-----------------------------------------
        int argc=0;
        char* token=strtok(input, " \t\n");
        while(token!=NULL && argc<63){
            args[argc++]=token;
            token=strtok(NULL," \t\n");
        }
        args[argc]=NULL; //null terminate the 64th ch.

        if(argc==0){
            //empty
            //do nothing
            continue;
        }

        int should_log=1; //file to track if we should add to log

        if(strcmp(args[0],"hop")==0){
            char current_dir[1024];
            getcwd(current_dir, sizeof(current_dir));
            hop(argc, args);
            char new_dir[1024];
            getcwd(new_dir,sizeof(new_dir)); //will return where the current cwd pointer is
            if(strcmp(current_dir, new_dir)!=0){
                strcpy(prev_dir,current_dir);
            }
            // continue;
            // add_to_log(input_copy);
            
        }

//-------------------------LLM Generated Code Ends--------------------------------------------
        else if(strcmp(args[0],"reveal")==0){
            reveal(argc, args, prev_dir);
            // continue;
            // add_to_log(input_copy);
            should_log=1;
        }

        else if(strcmp(args[0],"log")==0){
            if(argc==1){
                //just log, print everything
                print_log();
            }
            else if(argc==2 && (strcmp(args[1],"purge")==0)){
                purge_log();
            }
            else if(argc==3 && (strcmp(args[1],"execute")==0)){
                int index=atoi(args[2]); //atoi converts string to an integer
                // printf("index: %d\n",index);
                if(index>0){
                    // printf("1\n");
                    execute_log(index);
                }
                else{
                    fprintf(stderr,"Index overflow\n"); //fprintf writes output in the given stream here stderr
                }
            }
            else{
                fprintf(stderr,"log: Invalid Syntax!\n");
            }
            should_log=0;
        }   
        else if(strcmp(args[0],"activities")==0){
            activities();
            should_log=0;
        }
        else if(strcmp(args[0],"ping")==0){
            ping_command(argc,args);
            should_log=0;
        }
        else{

            if(strchr(input_copy,';')!=NULL){
                sequential_execute(input_copy);
                should_log=1;
                // continue;
            }

            else{
                 //check if the command contains a pipe
                int has_pipe=0;
                for(int i=0;i<argc;i++){
                    if(strcmp(args[i],"|")==0){
                        has_pipe=1;
                        break;
                    }
                }
                if(has_pipe){
                    if(pipeline(argc,args)==-1){
                        fprintf(stderr, "Pipeline execution failed\n");
                    }
                    should_log=1;
                }
                else{
                    //background execution check
                    should_log=1;

    //-------------------LLM Generated Code Begins--------------------------------------------
                    int background=0;
                    if(argc>0 && strcmp(args[argc-1],"&")==0){
                        background=1;
                        args[argc-1]=NULL;
                        argc--;
                    }   
                    
    // -----------------LLM Generated Code Ends--------------------------------------

                    if(parse_shell(input_copy)){
                        if(background){
                            run_in_background(argc, args, input_copy);
                        }
                        else{
                            //normal foreground execution
                            pid_t pid=fork();
                            if(pid==0){
                                if(output_redirection(&argc,args)<0){
                                    // printf("%s\n",args[1]);
                                    exit(1); //failed redirection
                                }
                                if(input_redirection(&argc,args)<0){
                                    // printf("%s\n",args[1]);
                                    exit(1); //failed redirection
                                }
                            
                                // printf("Valid syntax\n");
                                if(execute_arbitrary_commands(argc,args)==1){
                                    exit(0);
                                }
                                else{
                                    // fprintf(stderr,"Command not executed\n");
                                    exit(1);
                                }
                        
                            }
                            else if(pid>0){
                                foreground_pgid = pid;
                                setpgid(pid, pid); // optional if not done in child
                                waitpid(pid, NULL, WUNTRACED); // allows catching stopped process
                                foreground_pgid = -1; // reset after process finishes/stopped
                                // wait(NULL);
                            }
                            else{
                                perror("Fork failed\n");
                            }
                        }
                        // add_to_log(input_copy);
                        
                    }
                    else{
                        printf("Invalid Syntax!\n");
                        should_log=0;
                        }
                    }
                }
                should_log=1;
            
            }
            if(should_log){
                add_to_log(input_copy);
            }
        
       
    }
    log_free();
    return 0;

}
