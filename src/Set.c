#include "../include/Set.h"
#include "../include/Node.h" 
Set* createSet(){
    return NULL;
}

void set_addElem(Set **set, Node *node) {
    if(set_existElem(*set, node)) return ;
    
    Set *newElem = (Set*)malloc(sizeof(Set));
    newElem->node = node;
    newElem->next = *set;
    *set = newElem;
}

int set_existElem(Set *set, Node *node) {
    Set* currentElem = set;
    while (currentElem != NULL)
    {
        if(currentElem->node == node) return 1;
        currentElem = currentElem->next;
    }
    return 0;
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

Set* set_nextElem(Set *set){
    if (!set)
        return NULL;
    return set->next;
}

Set* set_difference(Set *current, Set *old) {
    Set *result = createSet();

    while(current != NULL) {
        if(!set_existElem(old, current->node)) {
            set_addElem(&result, current->node);
        }
        current = set_nextElem(current);
    }
    return result;
}


Set* set_union(Set *set1, Set *set2) {
    // Resultado empieza vacio
    Set *result = createSet();
    Set *cur;

    // agregamos todos los elementos de set1 y set2
    cur = set1;
    while (cur != NULL) {
        set_addElem(&result, cur->node);
        cur = cur->next;
    }

    cur = set2;
    while (cur != NULL) {
        set_addElem(&result, cur->node);
        cur = cur->next;
    }

    return result;
}

Set* set_clone(Set *src) {
    Set *clone = NULL;
    Set *cur = src;
    while (cur) {
        // set_addElem ya comprueba duplicados, pero como clone empieza NULL
        set_addElem(&clone, cur->node);
        cur = cur->next;
    }
    return clone;
}


void set_destroy(Set *s) {
    Set *cur = s;
    while (cur) {
        Set *next = cur->next;
        free(cur);
        cur = next;
    }
}

void set_print(Set *set) {
    while (set) {
        printf("- %s\n", set->node->name);
        set = set->next;
    }
}