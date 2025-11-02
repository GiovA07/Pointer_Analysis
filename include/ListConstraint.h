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


extern ListConstraint *listComplex1;  // a ⊇ *b
extern ListConstraint *listComplex2;  // *a ⊇ b


void addConstraint(ListConstraint **list, Node *a, Node *b);
Node* constraint_getL (ListConstraint *list);
void constraint_setL(ListConstraint *list, Node *rep);
Node* constraint_getR (ListConstraint *list);
void constraint_setR(ListConstraint *list, Node *rep);
Set* constraint_getCache(ListConstraint *list);
void constraint_setCache(ListConstraint *list, Set *newCache);

void constraints_remap_nodes(ListConstraint *list, Node *oldw, Node *rep);

void printConstraintComplex1(ListConstraint *list);
void printConstraintComplex2(ListConstraint *list);
#endif
