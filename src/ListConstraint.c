#include "../include/ListConstraint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Add List Constrraint
void addConstraint(ListConstraint **list, char *l_name, char *r_name) {
    ListConstraint *new_c = (ListConstraint *)malloc(sizeof(ListConstraint));
    if (!new_c) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la restricción.\n");
        return;
    }
    new_c->l = strdup(l_name);
    new_c->r = strdup(r_name);
    new_c->pcache = createSet();
    new_c->next = *list;  
    *list = new_c;
}

char* constraint_getL (ListConstraint *list) {
    return list->l;
}

char* constraint_getR (ListConstraint *list) {
    return list->r;
}

Set* constraint_getCache(ListConstraint *list) {
    return list->pcache;
}

void constraint_setL(ListConstraint *list, char *l_name) {
    free(list->l);
    list->l = strdup(l_name);
}

void constraint_setR(ListConstraint *list, char *r_name) {
    free(list->r);
    list->r = strdup(r_name);
}
void constraint_setCache(ListConstraint *list, Set *newCache) {
    if (!list) return;
    Set *old = list->pcache;
    list->pcache = newCache;
    if (old && old != newCache) 
        set_destroy(&old);
}


static void constraint_resetCache(ListConstraint *c) {
    set_destroy(&c->pcache);
    c->pcache = createSet();
}

void constraints_reset_all_caches(ListConstraint *list) {
    for (ListConstraint *c = list; c; c = c->next) {
        constraint_resetCache(c);
    }
}

void constraints_remap_cache(ListConstraint *list, Node *oldw, Node *rep) {
    for (ListConstraint *c = list; c; c = c->next) {
        // 1) remapear pcache (sigue siendo Set de Node*)
        if (set_existElem(c->pcache, oldw)) {
            set_deleteElem(&c->pcache, oldw);
            set_addElem(&c->pcache, rep);
        }
    }
}

// a ⊇ ∗b
void printConstraintComplex1(ListConstraint *list) {
    printf("Complex 1 Constraints:\n");
    for ( ; list; list = list->next)
        printf("%s ⊇ *%s\n", list->l, list->r);
}

// ∗a ⊇ b
void printConstraintComplex2(ListConstraint *list) {
    printf("Complex 2 Constraints:\n");
    for ( ; list; list = list->next)
        printf("*%s ⊇ %s\n", list->l, list->r);
}

void constraints_destroy(ListConstraint *list) {
    if (!list) return;
    while (list) {
        ListConstraint *nx = list->next;
        free(list->l);
        free(list->r);
        set_destroy(&list->pcache);
        free(list);
        list = nx;
    }
}
