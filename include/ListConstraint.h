#ifndef LISTCONSTRAINT_H
#define LISTCONSTRAINT_H

#include "Node.h"
#include "Set.h"

typedef struct ListConstraint {
    Node *superset;                     // 'a' en a ⊇ *b o *a ⊇ b
    Node *dereferenced;                 // 'b' en *b o b
    Set *pcache;                        // conjunto cache de nodos ya procesados
    struct ListConstraint *next;        // Siguiente restriccion compleja
} ListConstraint;

void addConstraint(ListConstraint **list, Node *a, Node *b);
Node* constraint_getL (ListConstraint *list);
Node* constraint_getR (ListConstraint *list);
Set* constraint_getCache(ListConstraint *list);

void constraint_setCache(ListConstraint *list, Set *newCache);

ListConstraint* constraint_getNext(ListConstraint *list);

void printConstraintComplex1(ListConstraint *list);
void printConstraintComplex2(ListConstraint *list);
#endif
