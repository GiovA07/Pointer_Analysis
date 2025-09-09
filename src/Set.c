#include "../include/Set.h"
#include "../include/Node.h" 
Set* createSet(){
    return NULL;
}

bool set_existElem(Set *set, Node *node) {
    for (Set *cur = set; cur; cur = cur->next)
        if (cur->node == node) return true;
    return false;
}

bool set_addElem(Set **set, Node *node) {
    if(set_existElem(*set, node)) return false;
    
    Set *newElem = (Set*)malloc(sizeof(Set));
    newElem->node = node;
    newElem->next = *set;
    *set = newElem;
    return true;
}

void set_deleteElem(Set **set, Node *node) {
    if(!set_existElem(*set, node)) return ;

    Set *prev = NULL;
    Set* currentElem = *set;
    while (currentElem != NULL)
    {
        if(currentElem->node == node) {
            if (prev == NULL)
                *set = currentElem->next;
            else
                prev->next = currentElem->next;
        }
        prev = currentElem;
        currentElem = currentElem->next;
    }
}

Set* set_nextElem(Set *set) {
    return set ? set->next : NULL;
}

Set* set_difference(Set *current, Set *old) {
    Set *result = createSet();
    for (Set *c = current; c; c = c->next) {
        if (!set_existElem(old, c->node))
            set_addElem(&result, c->node);
    }
    return result;
}

Set* set_union(Set *set1, Set *set2) {
    // Resultado empieza vacio
    Set *result = createSet();
    Set *cur;

    // agregamos todos los elementos de set1 y set2
    for (Set *c = set1; c; c = c->next)
        set_addElem(&result, c->node);

    for (Set *c = set2; c; c = c->next)
        set_addElem(&result, c->node);

    return result;
}

Set* set_clone(Set *src) {
    Set *clone = NULL;
    for (Set *c = src; c; c = c->next)
        set_addElem(&clone, c->node);
    return clone;
}

void set_destroy(Set *s) {
    while (s) {
        Set *next = s->next;
        free(s);
        s = next;
    }
}

bool set_empty(Set *s) { return s == NULL; }


void set_print(Set *set) {
    for (; set; set = set->next)
        printf("- %s\n", set->node->name);
}