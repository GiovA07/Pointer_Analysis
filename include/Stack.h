#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Node.h"

#ifndef STACK_H
#define STACK_H

typedef struct Stack {
    Node *top;
    struct Stack *next;
} Stack;

// Funciones de la pila
Node* first(Stack *stack);
Node* pop(Stack **stack);
void push(Stack **stack, Node *node);

#endif // STACK_H
