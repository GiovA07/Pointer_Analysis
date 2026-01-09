#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Como no estoy utilizando los campos de node, no necesito saberlos, ni que tamaño tienen*/
typedef struct Node Node;

typedef struct StackNode {
    Node *node;
    struct StackNode *next;
} StackNode;

typedef struct Stack {
    StackNode *top;
} Stack;

// Funciones de la pila
Stack* stack_create();
bool stack_push(Stack *stack, Node *node);
Node* stack_top(Stack *stack);
bool stack_pop(Stack *stack);
bool stack_isEmpty(Stack *stack);
void stack_destroy(Stack **stack);

#endif // STACK_H
