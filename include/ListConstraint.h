#ifndef LISTCONSTRAINT_H
#define LISTCONSTRAINT_H

#include "Node.h"
#include "Set.h"

typedef struct ListConstraint {
    char *l;                     // 'a' en a ⊇ *b o *a ⊇ b
    char *r;                     // 'b' en *b o b
    Set *pcache;                        // conjunto cache de nodos ya procesados
    struct ListConstraint *next;        // Siguiente restriccion compleja
} ListConstraint;


extern ListConstraint *listComplex1;  // l ⊇ *r
extern ListConstraint *listComplex2;  // *l ⊇ r


void addConstraint(ListConstraint **list, char *l_name, char *r_name);

char* constraint_getL (ListConstraint *list);
char* constraint_getR (ListConstraint *list);

void constraint_setL(ListConstraint *list, char *rep);
void constraint_setR(ListConstraint *list, char *rep);

Set* constraint_getCache(ListConstraint *list);
void constraint_setCache(ListConstraint *list, Set *newCache);


void constraint_resetCache(ListConstraint *c);
void constraints_reset_all_caches(ListConstraint *list);

void constraints_remap_cache(ListConstraint *list, Node *oldw, Node *rep);

void printConstraintComplex1(ListConstraint *list);
void printConstraintComplex2(ListConstraint *list);

void constraints_destroy(ListConstraint *list);
#endif
