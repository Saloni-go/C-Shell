// Parser.c

//Tokenizer converts the input string into tokens (words, |, ;, <, >, <<, >>, etc)
//Parser checks whether the sequence of tokens is a valid shell command acc to the grammar rules.


// -----------------LLM Generated Code Begins------------------------------------------------------------------------------------------------------------------

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include "../include/parser.h"

typedef enum{
    TOK_NAME,
    TOK_PIPE,  // |
    TOK_SEMI,  // ;
    TOK_AMP,   // &
    TOK_INPUT, // <
    TOK_OUTPUT,// >
    TOK_APPEND,// >>
    TOK_END
}TokenType;

typedef struct{
    TokenType type;
    char value[256];
}Token;

//skip whitespace
static void skip_ws(const char** s){
    //s  -> string
    //*s -> pointer to the string
    //**s-> current character of the string
    while(isspace(**s)){
        (*s)++;
    }
}

static Token tokens[1024]; //array to hold all the tokens
static int pos=0; //current parsing position
static int token_count=0;  //how many tokens we got after tokenization

static void tokenize(const char* input){
    token_count=0;
    pos=0;
    const char* p=input;

    while(*p){
        skip_ws(&p);
        if(*p=='\0'){
            break;
        }
        if(*p=='|'){
            tokens[token_count]=(Token){TOK_PIPE, "|"};
            token_count++;
            p++;
        }

//---------------------LLM Generated code ends-------------------------------------------------------------------------------------------------------------------------

        else if(*p==';'){
            tokens[token_count]=(Token){TOK_SEMI,";"};
            token_count++;
            p++;
        }
        else if(*p=='&'){
            tokens[token_count]=(Token){TOK_AMP,"&"};
            token_count++;
            p++;
        }
        else if(*p=='<'){
            tokens[token_count]=(Token){TOK_INPUT,"<"};
            token_count++;
            p++;
        }
        else if(*p=='>'){
            if(*(p+1)=='>'){
                tokens[token_count]=(Token){TOK_APPEND,">>"};
                p++;
            }
            else{
                tokens[token_count]=(Token){TOK_OUTPUT,">"};
            }
            token_count++;
            p++;
        }
        else{
            //no characters or whitespace, just words

//------------------ LLM Generated Code Begins------------------------------------------------------------------------------------------------------------------------

            char buff[1024]; //stores all the words separated by whitespaces
            int i=0;
            while(*p && !isspace(*p) && (*p!=';') && (*p!='|') && (*p!='>') && (*p!='<') && (*p!='&')){
                buff[i]=*p;
                *p++,i++;
            }
            buff[i]='\0';
            tokens[token_count]=(Token){TOK_NAME,""};  //first put a empty string so that buffer can be copied in this string
            strcpy(tokens[token_count].value,buff);
            token_count++;
        }

    }
    tokens[token_count++]=(Token){TOK_END,""};

//------------------ LLM Generated Code Ends -------------------------------------------------------------------------------------------------------------------------------

}

// Parser
// peek(): looking at the next character in the input without removing it.
// get():  read the next character and also move forward.

static Token *peek(){
    return &tokens[pos];
}
static Token *get(){
    return &tokens[pos++];
}

static int parse_atomic(){
    //atomic->name(name|input|output)*
    if(peek()->type!=TOK_NAME){
        return 0;
    }
    get(); //takes the first cmd input

    while(1){
        if(peek()->type==TOK_NAME){
            get();
        }
        else if(peek()->type==TOK_INPUT){
            get();
            if(peek()->type==TOK_NAME){
                get();
            }
            else return 0;
        }
        else if(peek()->type==TOK_OUTPUT){
            get();
            if(peek()->type==TOK_NAME){
                get();
            }
            else return 0;
        }
        else if(peek()->type==TOK_APPEND){
            get();
            if(peek()->type==TOK_NAME){
                get();
            }
        
            else return 0;
        }
    
        else{
            break;
        }
    }
    return 1;
}

static int parse_cmd(){
    //cmd_group->atomic(\|atomic)*
    if(parse_atomic()==0) return 0;

    while(peek()->type==TOK_PIPE){
            get();

            if(parse_atomic()!=0){}
            else{
                return 0;
            }
    }
    return 1;
}

static int parse_shell_internal(){
    //shell_cmd->cmd_group((&|;)cmd_group)*&?
    if(parse_cmd()==0) return 0;
    int flag=0, amp=0;
   
    while((peek()->type==TOK_AMP) ||  (peek()->type==TOK_SEMI)){
        if(peek()->type==TOK_AMP){
            amp=1;
            get();
            if(peek()->type==TOK_END){
                return 1;
            }
        }
        if(amp==0){
            get(); //get for tok_semi case also
        }

        if(parse_cmd()!=0){
            //do nothing
        }
        else return 0;
    }
    
    if(peek()->type==TOK_AMP){
        get(); //optional so no else return 0 case
    }
    if(peek()->type!=TOK_END) return 0;

    return 1;
}

// ------------------------- LLM Generated Code Begins ----------------------------------------------------------

int parse_shell(const char* input){
    tokenize(input);
    return parse_shell_internal();
}

//--------------------------- LLM Generated Code Ends ---------------------------------------------------------------
