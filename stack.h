/* Author: Blake Molina
   Assignment: Project 3 - UNIX Shell & History Feature
   CWID: 890198401
   Class: CPSC-351 Section 2
   Date: 19 September, 2017

   Stack Class: used for displaying shell command history
 */
#ifndef UNTITLED1_STACK_H
#define UNTITLED1_STACK_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct stackNode stackNode;
typedef struct stack stack;

struct stackNode{
    stackNode *next;
    char *command;
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

stackNode * newStackNode(const char *str){
    stackNode *newNode = (stackNode*)malloc(sizeof(stackNode));
    newNode->command = (char*)malloc(strlen(str)*sizeof(char));
    strcpy(newNode->command, str);
    newNode->next = NULL;
    return newNode;
}

void push(stack *s, const char *str){
    if(isEmpty(s))
        s->head = newStackNode(str);
    else{
        stackNode *temp = newStackNode(str);
        temp->next = s->head;
        s->head = temp;
    }
    ++s->numNodes;
}

void pop(stack *s){
    if(isEmpty(s)) return;
    stackNode *delNode = s->head;
    s->head = s->head->next;
    delNode->next = NULL;
    free(delNode->command);
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
        printf("%zu %s\n", count, curr->command);
        --count;
        curr = curr->next;
    }
}

#endif //UNTITLED1_STACK_H
