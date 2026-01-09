// opendir(path): opens the directory stream for the given path
// readdir(path): reads one entry (a file or subdirectory) from the open directory. Each entry is returned as a struct dirent*
// closedir(dir): closes the directory stream

//DIR is a data type defined in <dirent.h> which represents an open directory stream (kind of like FILE* for files).
//struct dirent* entry represents one entry in a directory (it can be a file, a subdirectory, or even special entries like . and ..)

// struct dirent{
//     d_ino       i.e. inode number   
//     d_off       i.e. offset to next dirent
//     d_reclen    i.e. length of this record
//     d_type      i.e. type of file (regular, dir, link, etc.)
//     d_name[256] i.e. filename (null terminated)  
// }

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "../include/reveal.h"
#include<dirent.h> //for opendir, readdir, and closedir


//----------------------------LLM Generated Code Begins-----------------------------------
int compare_func(const void* a, const void* b){
    return strcmp(*(const char** )a,*(const char**)b); //retunrs 0 if a=b, +ve value if a>b, -ve value if a<b.
}

// More specific version - only ignore operators that would cause reveal to fail
int is_shell_operator(const char* token) {
    // Stop processing when we hit any shell operator
    // This prevents reveal from treating operators as directory arguments
    return (strcmp(token, "|") == 0 ||
            strcmp(token, ">") == 0 ||
            strcmp(token, ">>") == 0 ||
            strcmp(token, "<") == 0 ||
            strcmp(token, ";") == 0 ||
            strcmp(token, "&") == 0 ||
            strcmp(token, "&&") == 0 ||     // Logical AND (if you implement it)
            strcmp(token, "||") == 0);      // Logical OR (if you implement it)
}

int reveal(int argc, char* argv[], const char* prev_dir){
    int a_flag=0;
    int l_flag=0;
    char* dir_path=NULL;
    int dir_count=0;

    //argv is a pointer to strings eg. argv[0]=reveal so argv[0][1]=r
    for(int i=1;i<argc;i++){  //i=0 is reveal
        if(is_shell_operator(argv[i])){
            break; // Stop processing arguments when we hit an operator
        }
        if(argv[i][0]=='-'){
            for(int j=1;j<strlen(argv[i]);j++){
                if(argv[i][j]=='a'){
                    a_flag=1;
                }
                else if(argv[i][j]=='l'){
                    l_flag=1;
                }
                //else ignore 
            }
        }
        else{//if not a -
            dir_count++;
            if(dir_count>1){  //reveal cant have a lot of arguments, so here we are specifying that after -al we can have one dir_arg. 
                printf("reveal: Invalid Syntax!\n");
                return -1;
            }
            dir_path=argv[i];
        }
    }

    //if argv[i] i.e. dir_path null that means current directory itself
    if(dir_path==NULL){
        dir_path=".";
    }

    char resolved_path[1024];
    if(strcmp(dir_path,"~")==0){
        const char* home=getenv("HOME");
        if(!home){
            printf("No such directory\n");
            return -1;
        }
        // strcpy(dir_path,home); cant do this directly as argv[i] is read only memory.
        strcpy(resolved_path,home);
        dir_path=resolved_path;
    }
    else if(strcmp(dir_path,"-")==0){
        if((prev_dir==NULL) || (strlen(prev_dir)==0)){
            printf("No such directory\n");
            return -1;
        }
        // strcpy(dir_path,prev_dir);
        strcpy(resolved_path,prev_dir);
        dir_path=resolved_path;
    }
    else if(strcmp(dir_path, "..")==0){
        // .. is a valid path
    }
    else if(strcmp(dir_path, ".")==0){
        // . is a valid path
    }
    //else if its a name that is also a valid path

    //OPEN DIRECTORY
    DIR* dir=opendir(dir_path);
    if(!dir){
        printf("No such directory!\n");
        return -1;
    }

    struct dirent* entry; //will store  one files/directories names
    char* files[1024]; //stores list of strings that will store all filenames
    int count=0; //keeps track of how many files/directories we have stored till now. 
    
    while((entry=readdir(dir))!=NULL){
        if(!a_flag && entry->d_name[0]=='.'){ //any file starting with . is hidden
            continue; 
        }
        files[count]=strdup(entry->d_name);//else duplicate the name of file into the file array
        count++;

    }
    closedir(dir);

    qsort(files, count, sizeof(char*), compare_func);

    for(int i=0;i<count;i++){
        if(l_flag){
            printf("%s\n", files[i]);
        }
        else{
            printf("%s",files[i]);
            if(i < count - 1){
                printf(" ");
            }
        }
        free(files[i]);
    }

    if(!l_flag && count>0) printf("\n");


    return 0;
}

//-------------------------- LLM Generated Code Ends --------------------------------------