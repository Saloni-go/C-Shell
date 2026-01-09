#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

//--------------------------------LLM Generated Code Begins-----------------------------------

static char prev_dir[1024]="";

char* expand_path(const char* path) {
    if (path[0] == '~') {
        const char* home = getenv("HOME");
        if (!home) return strdup(path);

        char* expanded = malloc(strlen(home) + strlen(path)); 
        if (!expanded) return NULL;

        strcpy(expanded, home);
        strcat(expanded, path + 1); // skip the '~'
        return expanded;
    }
    return strdup(path); // no expansion needed
}


int hop(int argc, char* argv[]){

    char current_dir[1024];
    if(getcwd(current_dir, sizeof(current_dir))==NULL){
        printf("Error getting current directory\n");
    }

    if(argc==1){
        //no arguments i.e. argv[0]=hop
        //treat is as ~, and go to home directory
        const char* home=getenv("HOME"); //returns a pointer to the environmental variable i,e here fetches the path to home
        if(home && chdir(home)==0){
            strcpy(prev_dir,current_dir);
        }
        else{
            printf("No such directory!\n");
        }
        return 0;
    }

    //more than one arguments
    for(int i=1;i<argc;i++){ //i=0 is hop, leave it.
        char* arg=argv[i];
        getcwd(current_dir, sizeof(current_dir));

        char* target = expand_path(argv[1]);  // handle ~ and ~/subdir
        if (!target) {
            fprintf(stderr, "Memory error\n");
            return -1;
        }

        if(strcmp(target,"~")==0){
            if (strlen(prev_dir) > 0) {
                char tmp[1024];
                strcpy(tmp, current_dir);
                if (chdir(prev_dir) == 0) {
                    strcpy(prev_dir, tmp);
                } else {
                    printf("No such directory!\n");
                }
            }
        }

        else if(strcmp(arg,".")==0){
            //do nothing
        }
        else if(strcmp(arg, "..")==0){ // .. means parent directory in UNIX
            if(chdir("..")==0){ //0 if successful and -1 if it fails.
                strcpy(prev_dir,current_dir);
            }
        }
        else if(strcmp(arg, "-")==0){
            if((strlen(prev_dir))>0){
                char temp[1024]="";
                strcpy(temp, current_dir);
                if (chdir(prev_dir) == 0) {
                    strcpy(prev_dir, temp);
                } else {
                    printf("No such directory!\n");
                }
            }
        }
        else{
            if(chdir(arg)==0){
                strcpy(prev_dir,current_dir);
            }
            else{
                printf("No such directory!\n");
            }
        }
    }

    return 0;

}

// ------------------------LLM Generated Code Ends ------------------------------------------