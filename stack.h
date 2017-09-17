/* Author: Blake Molina
 *  Assignment: Project 3 - UNIX Shell & History Feature
 *  CWID: 890198401
 *  Class: CPSC-351 Section 2
 *  Date: 19 September, 2017
 *
 *  Stack Class: used for displaying shell command history
 */
#ifndef HARP_SHELL_STACK_H
#define HARP_SHELL_STACK_H


#include <stdbool.h>
#include "common.h"

typedef struct stackNode stackNode;
typedef struct stack stack;

struct stackNode{
    stackNode *next;
    char *args[MAX_ARGUMENTS];
};

struct stack{
    stackNode *head;
    size_t numNodes;
};

bool isEmpty(stack *s){
    if(s == NULL || s->head == NULL)
        return true;
    return false;
}

void initializeStack(stack *s){
    if(s){
        s->head = NULL;
        s->numNodes = 0;
    }
}

stackNode * newStackNode(char *args[]){
    stackNode *newNode = (stackNode*)malloc(sizeof(stackNode));
    /* Initialize args array of stack node to NULL */
    for(int i = 0; i < MAX_ARGUMENTS; i++) newNode->args[i] = NULL;
    /* Copy each command into the stack nodes command array */
    for(int i = 0; i < MAX_ARGUMENTS; i++){
        if(args[i] != NULL){
            newNode->args[i] = (char*)malloc(strlen(args[i])*sizeof(char));
            strcpy(newNode->args[i], args[i]);
        }
    }
    newNode->next = NULL;
    return newNode;
}

void push(stack *s, char *args[]){
    if(isEmpty(s))
        s->head = newStackNode(args);
    else{
        stackNode *temp = newStackNode(args);
        temp->next = s->head;
        s->head = temp;
    }
    ++s->numNodes;
}

void pop(stack *s){
    if(isEmpty(s)) return;
    stackNode *delNode = s->head;
    for(int i = 0; i < MAX_ARGUMENTS; i++){
        if(s->head->args[i] != NULL)
            free(s->head->args[i]);
    }
    s->head = s->head->next;
    delNode->next = NULL;
    free(delNode);
    --s->numNodes;
}

void popAll(stack *s){
    while(!isEmpty(s)){ pop(s); }
}

void freeStack(stack **s){
    if(!s || !*s) return;
    popAll(*s);
    free(*s);
    *s = NULL;
}

void print(stack *s){
    if(isEmpty(s)) return;
    stackNode *curr = s->head;
    size_t count = s->numNodes;
    while(curr){
        printf("%zu ", count);
        for(int i = 0; i < MAX_ARGUMENTS; i++) {
            if (curr->args[i] != NULL)
                printf("%s ", curr->args[i]);
        } printf("\n");
        --count;
        curr = curr->next;
    }
}

#endif //HARP_SHELL_STACK_H
