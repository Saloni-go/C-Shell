//PROMPT LOGIC 
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pwd.h>
#include<string.h>
#include<limits.h>
#include "../include/prompt.h"

char* build_prompt(const char* base_dir_name){

    //------------------LLM Generated Code Begins -----------------------

    //USERNAME
    struct passwd *pw=getpwuid(getuid()); //getuid returns the user id of the calling process, getpwuid returns a pointer to the password structure of given pid.
    //this struct includes pw_name (username), pw_passwd(user password), pw_uid(user ID), pw_gid (groupID), pw_gecos(user information), pw_dir(home directory), pw_shell(shell program)
    if(pw==NULL){
        //struct empty
        perror("getpwuid");  //perror prints in stderr and printf in stdout
        exit(1);
    }
    // printf("%s\n",pw->pw_name);

    //-------------------LLM Generated Code Ends --------------------------

    //HOSTNAME/SYSTEM NAME
    int len_host_name=256;
    char host_name[len_host_name];

    int h_name=gethostname(host_name,len_host_name);   //(char *name, size_t size)
    //on success 0 is returned, and on error -1 is returned with errno setting to indicate the error
    if(h_name==-1){
        perror("gethostname");
        exit(1);
    }
   
    // printf("%s\n", host_name);

    //CURRENT_PATH
    char current_path_name[256];
    char*c_name=getcwd(current_path_name,256);
    if(c_name==NULL){
        perror("getcwd");
        exit(1);
    }
    // printf("%s\n", current_path_name);

    char final_path_name[256];
    int count=1; //0th index tilda ~ 
    int flag=0; //to see if we have to use tilda or not
    // now compare current path with the base_dir_path and see if we are actually inside src or not
    if(strlen(base_dir_name)>strlen(current_path_name)){
        strcpy(final_path_name, current_path_name);
    }
    else{
        size_t i=0; //strlen length return size_t
        for(i=0;i<strlen(base_dir_name);i++){  
            if(base_dir_name[i]==current_path_name[i]){
                flag=1;
            }
            else{
                //not in src directory then
                strcpy(final_path_name, current_path_name);
                flag=0;
                break;
            }
        }
        if(flag==1){
            
            
            //both the strings are equal and equal to the path for home directory
            if(i==strlen(current_path_name)){
                strcpy(final_path_name,"~");
            }
            else{
                strcpy(final_path_name,"~");
                //stuff after src needs to be printed.
                for(size_t j=i+1;j<strlen(current_path_name);j++){
                    final_path_name[count]=current_path_name[j];
                    count++;
                }
            }
        }
    }
    
    // printf("%s\n", final_path_name);
    static char final_final_path_name[256];
    final_final_path_name[0]='\0'; //initializing empty string
    strcat(final_final_path_name, "<");
    strcat(final_final_path_name, pw->pw_name);
    strcat(final_final_path_name,"@");
    strcat(final_final_path_name, host_name);
    strcat(final_final_path_name, ":");
    strcat(final_final_path_name,final_path_name);
    strcat(final_final_path_name, "> ");

    return final_final_path_name;
}