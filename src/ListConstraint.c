#include "../include/ListConstraint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/Graph.h"

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
    newConstraint->pcache = createSet();
    *list = newConstraint;
}

Node* constraint_getL (ListConstraint *list) {
    return list->superset;
}

Node* constraint_getR (ListConstraint *list) {
    return list->dereferenced;
}

Set* constraint_getCache(ListConstraint *list) {
    return list->pcache;
}

void constraint_setL(ListConstraint *list, Node *rep) {
    list->superset = rep;
}

void constraint_setR(ListConstraint *list, Node *rep) {
    list->dereferenced = rep;
}
void constraint_setCache(ListConstraint *list, Set *newCache) {
    if (!list) return;
    Set *old = list->pcache;
    list->pcache = newCache;
    if (old && old != newCache) 
        set_destroy(old);
}

void constraints_remap_nodes(ListConstraint *list, Node *oldw, Node *rep) {
    for (ListConstraint *c = list; c; c = c->next) {
        if (c->superset     == oldw) c->superset     = rep;
        if (c->dereferenced == oldw) c->dereferenced = rep;
        if (set_existElem(c->pcache, oldw)) {
            set_deleteElem(&c->pcache, oldw);
            set_addElem(&c->pcache, rep);
        }
    }

    // Remapear el cache (reemplazar oldw por rep si estuviera)
    //no es necesario por ahora
}

// a ⊇ ∗b
void printConstraintComplex1(ListConstraint *list) {
    printf("Complex 1 Constraints:\n");
    for (; list; list = list->next)
        printf("%s ⊇ *%s\n", list->superset->name, list->dereferenced->name);
}

// ∗a ⊇ b
void printConstraintComplex2(ListConstraint *list) {
    printf("Complex 2 Constraints:\n");
    for (; list; list = list->next)
        printf("*%s ⊇ %s\n", list->superset->name, list->dereferenced->name);
}


