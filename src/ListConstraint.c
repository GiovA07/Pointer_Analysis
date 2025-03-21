#include "../include/ListConstraint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Add List Constrraint
void addConstraint(ListConstraint **list, Node *a, Node *b) {
    ListConstraint *newConstraint = (ListConstraint *)malloc(sizeof(ListConstraint));
    if (!newConstraint) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la restricción.\n");
        return;
    }
    newConstraint->superset = a;
    newConstraint->dereferenced = b;
    newConstraint->next = *list;  
    *list = newConstraint;
}

// a ⊇ ∗b
void printConstraintComplex1(ListConstraint *list) {
    printf("Complex 1 Constraints:\n");
    while (list) {
        printf("%s ⊇ *%s\n", list->superset->name, list->dereferenced->name);
        list = list->next;
    }
}

// ∗a ⊇ b
void printConstraintComplex2(ListConstraint *list) {
    printf("Complex 2 Constraints:\n");
    while (list) {
        printf("*%s ⊇ %s\n", list->superset->name, list->dereferenced->name);
        list = list->next;
    }
}

