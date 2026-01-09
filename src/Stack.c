
#include "../include/Stack.h"

Stack* stack_create() { 
    Stack *inicTop = (Stack *)malloc(sizeof(Stack));
    if (!inicTop) return NULL;
    inicTop->top = NULL;
    return inicTop;
}

bool stack_push(Stack *stack, Node *node) {
    if (!stack) return false;
    StackNode *newTop = (StackNode *)malloc(sizeof(StackNode));
    if (!newTop) return false;

    newTop->node = node;
    newTop->next = stack->top;
    stack->top = newTop;
    return true;
}

Node* stack_top(Stack *stack) {
    if (!stack || !stack->top) return NULL;
    return stack->top->node;
}

bool stack_pop(Stack *stack) {
    if (!stack || !stack->top) return false;
    StackNode *old = stack->top;
    stack->top = old->next;
    free(old);
    return true;
}

bool stack_isEmpty(Stack *stack) {
    return !stack || (stack->top == NULL);
}

/* Destructor total: limpia y libera la estructura Stack */
/* No elimina los nodos dentro del stack ya que estos son los mismos que en el grafo*/
void stack_destroy(Stack **stack) {
    if (!stack || !*stack) return;
    Stack *s = *stack;
    while (s->top) {
        StackNode *n = s->top;
        s->top = n->next;
        free(n);
    }
    free(s);
    *stack = NULL;
}
