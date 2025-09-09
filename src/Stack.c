
#include "../include/Stack.h"

Stack* createStack() { 
    Stack *inicTop = (Stack *)malloc(sizeof(Stack));
    if (!inicTop) return NULL;
    inicTop->top = NULL;
    return inicTop;
}

bool push(Stack *stack, Node *node) {
    if (!stack) return false;
    StackNode *newTop = (StackNode *)malloc(sizeof(StackNode));
    if (!newTop) return false;
    StackNode *oldTop = stack->top;
    newTop->node = node;
    newTop->next = stack->top;
    stack->top = newTop;
    return true;

}

void pop(Stack *stack) {
    if (!stack->top) {
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

bool isEmpty(Stack *stack) {
    return !stack || (stack->top == NULL);
}

/* Destructor total: limpia y libera la estructura Stack */
void destroyStack(Stack *stack) {
    if (!stack) return;
    free(stack);
}


