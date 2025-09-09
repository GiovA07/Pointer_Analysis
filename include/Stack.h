#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Node.h"


typedef struct StackNode {
    Node *node;
    struct StackNode *next;
} StackNode;

typedef struct Stack {
    StackNode *top;
} Stack;

// Funciones de la pila
Stack* createStack();
bool push(Stack *stack, Node *node);
void pop(Stack *stack);
Node* top(Stack *stack);
bool isEmpty(Stack *stack);
void destroyStack(Stack *stack);

#endif // STACK_H
