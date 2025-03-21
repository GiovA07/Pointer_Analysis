#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Node.h"

#ifndef STACK_H
#define STACK_H

typedef struct StackNode {
    Node *node;
    struct StackNode *next;
} StackNode;

typedef struct Stack {
    StackNode *top;
} Stack;

// Funciones de la pila
Stack* createStack();
void push(Stack *stack, Node *node);
void pop(Stack *stack);
Node* top(Stack *stack);
int isEmpty(Stack *stack);

#endif // STACK_H
