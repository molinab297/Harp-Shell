/* Author: Blake Molina
 * Assignment: Project 3 - UNIX Shell & History Feature
 * CWID: 890198401
 * Class: CPSC-351 Section 2
 * Date: 19 September, 2017
 */
#include "common.h"
#include "stack.h"

/* -----------------------------------------------------------------------------
 * Function parseUserInput
 *
 * Summary : Fills the args array with arguments input by the user
 * Input   : input - a char array (input string)
 *           args  - a pointer to an array of strings (arguments)
 *           delimiters - the string of delimiters on which input will be split
 * Returns : None
*/
void split(char input[], char **args, char *delimiters){
    int index = 0;
    /* Split string on every space/newline character */
    for (char *p = strtok(input,delimiters); p != NULL; p = strtok(NULL,delimiters)) {
        /* Store returned value from strtok() into argument array */
        char *token = strdup(p);
        args[index] = token;
        ++index;
    }
}

/*  -----------------------------------------------------------------------------
 *  Function execute
 *
 *   Summary : Forks a child process to execute a command given by the user
 *   Input   : args - an array of pointers to strings (the command arguments)
 *             background_flag - indicates whether to the run the command as a
 *             background process or not
 *   Returns : None
 */
void execute(char **args, int background_flag){
    pid_t child_pid;

    /* Fork child process */
    child_pid = fork();

    if (child_pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        exit(1);
    }

    if (child_pid == 0) { /* child process */
        if(execvp(args[0], args) < 0){
            fprintf(stderr, "Error - command failed to execute\n");
            exit(1);
        }
    }
    else{ /* parent process */
        /* wait for child process to finish */
        if(background_flag == 0) wait(NULL);
    }
}

/*  -----------------------------------------------------------------------------
 *  Function getNthCommand
 *
 *   Summary : Returns the argument array of the Nth history object
 *   Input   : s - a stack of command history objects
 *             n - an integer denoting the Nth args array to return
 *   Returns : an argument array
 */
char ** getNthCommand(stack *s, int n){
    if(isEmpty(s) || n > s->numNodes) return NULL;
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
void clearArgumentAr(char **args, int size){
    /* Clear argument array for next command */
    for(int i = 0; i < size; i++){
        if(args[i] != NULL) {
            free(args[i]);
            args[i] = NULL;
        }
    }
}

/*  -----------------------------------------------------------------------------
 *  Function loadShellHistory
 *
 *   Summary : loads command history from a text file
 *   Input   : history_stack - a pointer to stack object
 *             outputFileName - name of text file to load history from
 *   Returns : none
 */
void loadShellHistory(stack *history_stack, char outputFileName[]){
    if(!history_stack) return;

    FILE *file = fopen(outputFileName, "r");
    if(!file) return;

    char *args[MAX_ARGUMENTS] = {NULL};
    char line[BUFFER_SIZE];

    while(fgets(line, sizeof(line), file)){
        /* read history line by line */
        split(line, args, " &\n");
        /* push command onto stack */
        push(history_stack, args);
        /* clear argument array for the next command */
        clearArgumentAr(args, MAX_ARGUMENTS);
    }

    fclose(file);
}

/*  -----------------------------------------------------------------------------
 *  Function saveShellHistoryRecur
 *
 *   Summary : writes command history to a text file
 *   Input   : head - a pointer to the top of a stack object
 *             file - a pointer to a file object
 *   Returns : none
 */
void saveShellHistoryRecur(stackNode *head, FILE *file){
	if(head != NULL){
		saveShellHistoryRecur(head->next, file);
		for(int i = 0; i < MAX_ARGUMENTS; i++){
			if(head->args[i] != NULL)
				fprintf(file, "%s ", head->args[i]);
		} fprintf(file, "\n");
	}
}

/*  -----------------------------------------------------------------------------
 *  Function saveShellHistory
 *
 *   Summary : Prepares a file for writing command history to, and then calls a
 			   recursive function to write the command history to the file.
 			   (See @saveShellHistoryRecur).
 *   Input   : history_stack - a pointer to stack object
 *             outputFileName - name of text file to save history to
 *   Returns : none
 */
void saveShellHistory(stack *history_stack, char outputFileName[]){
    if(!history_stack) return;

    FILE *file = fopen(outputFileName, "w");
    if(!file){
        fprintf(stderr, "Failed to save file");
        exit(EXIT_FAILURE);
    }

    saveShellHistoryRecur(history_stack->head, file);
    
    fclose(file);
}


/* -----------------------------------------------------------------------------
 * Driver Function
 */
int main(){

    char history_log[] = "history.txt";
    char *args[MAX_ARGUMENTS] = {NULL};

    /* Create and initialize stack for shell command history */
    stack *historyStack = (stack*)malloc(sizeof(stack));
    initializeStack(historyStack);

    /* Load shell history from text file */
    loadShellHistory(historyStack, history_log);

    /* User exits shell by typing 'exit' */
    while (1) {

        /* Reads user input from stdin */
        char input[BUFFER_SIZE];
        do{
            printf("osh>");
            fflush(stdout);
            fgets(input, BUFFER_SIZE, stdin);
        } while(input[0] == '\n'); /* Handles case where user enters nothing */

        /* Check if user entered a '&' to run command in the background */
        int backgroundFlag = 0;
        if(input[strlen(input)-2] == '&') backgroundFlag = 1;

        /* Parse user input and store arguments in args array */
        split(input, args, " &\n");

        /* If user enters exit, terminate program */
        if(strcmp(args[0],"exit") == 0) break;

            /* Prints the users command history */
        else if(strcmp(args[0], "history") == 0)
            print(historyStack);
   
            /* Clears the history stack */
        else if(strcmp(args[0], "clear") == 0)
            popAll(historyStack);

            /* Executes !! or !# commands */
        else if(strstr(args[0], "!") != NULL && strlen(args[0]) <= 2){

            if(!isEmpty(historyStack)) {
                /* execute last command */
                if (args[0][1] == '!') {
                    execute(historyStack->head->args, backgroundFlag);
                    push(historyStack, historyStack->head->args);
                }
                    /* execute the Nth command */
                else {
                    char **nthCommand = getNthCommand(historyStack, args[0][1] - '0');
                    if(nthCommand != NULL) {
                        execute(nthCommand, backgroundFlag);
                        push(historyStack, nthCommand);
                    } else
                        fprintf(stderr, "No such command in history\n");
                }
            } else
                fprintf(stderr, "No commands in history\n");
        }

            /* Execute command normally */
        else{
            execute(args, backgroundFlag);
            push(historyStack, args);
        }

        /* Clears argument array for next user input */
        clearArgumentAr(args, MAX_ARGUMENTS);
    }

    /* Save shell history to a text file */
    saveShellHistory(historyStack, history_log);

    /* Delete command history */
    freeStack(&historyStack);

    return 0;
}