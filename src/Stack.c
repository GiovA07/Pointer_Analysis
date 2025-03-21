
#include "../include/Stack.h"

Stack* createStack() { 
    Stack *inicTop = (Stack *)malloc(sizeof(Stack));
    inicTop->top = NULL;
    return inicTop;
}

void push(Stack *stack, Node *node) {
    StackNode *newTop = (StackNode *)malloc(sizeof(StackNode));

    if (!newNode) {
        printf("Error \n");
        return;
    }

    StackNode *oldTop = stack->top;

    newTop->node = node;
    newTop->next = oldTop;
    stack->top = newTop;

}

void pop(Stack *stack) {
    if (stack->top == NULL) {
        printf("Pila vacía\n");
        return;
    }

    StackNode *oldTop = stack->top;
    stack->top = oldTop->next;
}

Node* top(Stack *stack) {
    if (stack->top == NULL) return NULL;
    return stack->top->node;
}

int isEmpty(Stack *stack) {
    return stack->top == NULL;
}


