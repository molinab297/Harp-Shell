/* Author: Blake Molina
   Assignment: Project 3 - UNIX Shell & History Feature
   CWID: 890198401
   Class: CPSC-351 Section 2
   Date: 19 September, 2017
 */
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include "stack.h"

#define MAX_ARGUMENTS 3
#define BUFFER_SIZE 25

/*
Function ReadUserInput

Summary : Fills the args array with arguments input by the user
Input   : An array of pointers to strings
Returns : None
*/
void readUserInput(char **args){
    char input[BUFFER_SIZE];
    fgets(input, BUFFER_SIZE, stdin); /* Read user input */
    int index = 0;
    /* Split string based on spaces and newline characters */
    for (char *p = strtok(input," \n"); p != NULL; p = strtok(NULL, " \n")) {
        char *token = strdup(p); /* Store returned value from strtok() */
        args[index] = token;
        ++index;
    }
}

/*
Function Execute

Summary : Forks a child process to execute a command given by the user
Input   : An array of pointers to strings
Returns : None
*/
void execute(char **args){
    pid_t pid;

    /* Fork child process */
    pid = fork();

    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        exit(1);
    }

    if (pid == 0) { /* child process */
        if(execvp(args[0], args) < 0){
            fprintf(stderr, "Error executing command");
            exit(1);
        }
    }
    else{ /* parent process */
        wait(NULL);
    }
}


int main(){

    char *args[MAX_ARGUMENTS] = {NULL}; /* command line arguments */
    stack *historyStack = (stack*)malloc(sizeof(stack)); /* Stack for shell command history */
    initializeStack(historyStack);

    while (1) {
        printf("osh>");
        fflush(stdout);

        /* Reads from stdin and stores result in args */
        readUserInput(args);

        /* If user enters exit, terminate program */
        if(strcmp(args[0],"exit") == 0) break;
        else if(strcmp(args[0], "history") == 0) print(historyStack);
        else if(args[0], "!!"){
            // execute last command
            if(!isEmpty(historyStack)){

            } else{
                fprintf(stderr, "No commands in history");
            }
        }
        else if(args[0], "!n"){
            // execute the nth command
            if(!isEmpty(historyStack)){

            } else{
                fprintf(stderr, "No commands in history");
            }
        }
        else if(strcmp(args[0], "clear") == 0) popAll(historyStack);
        else execute(args); /* Run command */

        /* Update command history */
        push(historyStack, args[0]);

        /* Clear argument array for next command */
        for(int i = 0; i < MAX_ARGUMENTS; i++){
            if(args[i]) free(args[i]);
            args[i] = NULL;
        }
    }

    /* Delete command history */
    freeStack(&historyStack);

    return 0;
}