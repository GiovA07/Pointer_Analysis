#ifndef LISTCONSTRAINT_H
#define LISTCONSTRAINT_H

#include "node.h"

typedef struct ListConstraint {
    Node *superset;                     // 'a' (el conjunto más grande en la restricción)
    Node *dereferenced;                 // '*b' (el nodo que se desreferencia)
    struct ListConstraint *next;        // Siguiente restriccion compleja
} ListConstraint;

void addConstraint(ListConstraint **list, Node *a, Node *b);
void printConstraintComplex1(ListConstraint *list);
void printConstraintComplex2(ListConstraint *list);


#endif
