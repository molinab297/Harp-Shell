/* Author: Blake Molina
 * Assignment: Project 3 - UNIX Shell & History Feature
 * CWID: 890198401
 * Class: CPSC-351 Section 2
 * Date: 19 September, 2017
 */
#include "common.h"
#include "stack.h"

/* -----------------------------------------------------------------------------
 * Function ReadUserInput
 *
 * Summary : Fills the args array with arguments input by the user
 * Input   : A pointer to an array of strings
 * Returns : None
*/
void parseUserInput(char input[], char **args){
    int index = 0;
    /* Split string based on spaces and newline characters */
    for (char *p = strtok(input," \n"); p != NULL; p = strtok(NULL, " \n")) {
        char *token = strdup(p); /* Store returned value from strtok() */
        args[index] = token;
        ++index;
    }
}

/*  -----------------------------------------------------------------------------
 *  Function Execute
 *
 *   Summary : Forks a child process to execute a command given by the user
 *   Input   : An array of pointers to strings
 *   Returns : None
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
            fprintf(stderr, "Error - command doesn't exist or failed to execute\n");
            exit(1);
        }
    }
    else{ /* parent process */
        wait(NULL);
    }
}

/*  -----------------------------------------------------------------------------
 *  Function getNthCommand
 *
 *   Summary : Returns the argument array of the Nth history object
 *   Input   : A stack of command history objects and an integer n, denoting the
 *             Nth args array to return
 *   Returns : an argument array
 */
char ** getNthCommand(stack *s, int n){
    if(isEmpty(s) || n > s->numNodes)return NULL;
    stackNode *curr = s->head;
    for(size_t i = 0; i < s->numNodes-n; i++){
        if(!curr) return NULL;
        curr = curr->next;
    }
    return curr->args;
}

/*  -----------------------------------------------------------------------------
 *  Function clearArgumentAr
 *
 *   Summary : Clears the argument array for the next command
 *   Input   : A pointer to an array of strings
 *   Returns : None
 */
void clearArgumentAr(char **args){
    /* Clear argument array for next command */
    for(int i = 0; i < MAX_ARGUMENTS; i++){
        if(args[i] != NULL) {
            free(args[i]);
            args[i] = NULL;
        }
    }
}


/* -----------------------------------------------------------------------------
 * Driver Function
 */
int main(){

    char *args[MAX_ARGUMENTS] = {NULL}; /* command line arguments */
    stack *historyStack = (stack*)malloc(sizeof(stack)); /* Stack for shell command history */
    initializeStack(historyStack);

    /* User exits shell by typing 'exit' */
    while (1) {
        printf("harp>");
        fflush(stdout);

        /* Reads user input from stdin */
        char input[BUFFER_SIZE];
        fgets(input, BUFFER_SIZE, stdin);

        /* Parse user input and store arguments in args array */
        parseUserInput(input, args);

        /* If user enters exit, terminate program */
        if(strcmp(args[0],"exit") == 0) break;
        else if(strcmp(args[0], "history") == 0) {
            print(historyStack);
            push(historyStack, args);
        }
            /* Executes !! or !# commands */
        else if(strstr(args[0], "!") != NULL && strlen(args[0]) <= 2){
            if(!isEmpty(historyStack)) {
                /* execute last command */
                if (args[0][1] == '!') {
                    execute(historyStack->head->args);
                    push(historyStack, historyStack->head->args);
                }
                /* execute Nth command */
                else {
                    char **nthCommand = getNthCommand(historyStack, args[0][1] - '0');
                    execute(nthCommand);
                    push(historyStack, nthCommand);
                }
            } else{
                fprintf(stderr, "Command doesn't exist in history\n");
            }
        }
        else if(strcmp(args[0], "clear") == 0) {
            popAll(historyStack);
            push(historyStack, args);
        }
        else{
            execute(args);  /* Run command */
            push(historyStack, args);
        }

        clearArgumentAr(args);
    }

    /* Delete command history */
    freeStack(&historyStack);

    return 0;
}