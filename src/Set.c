#include "../include/Set.h"

Set* createSet(){
    return NULL;
}

void addToCSet(Set **set, Node *node) {
    if(existsInSet(*set, node)) return ;

    Set *newElem = (Set*)malloc(sizeof(Set));
    newElem->node = node;
    newElem->next = *set;
    *set = newElem;
}

int existsInSet(Set *set, Node *node) {
    Set* currentElem = set;
    while (currentElem != NULL)
    {
        if(currentElem->node == node) return 1;
        currentElem = currentElem->next;
    }
    return 0;
}

void deleteFromSet(Set **set, Node *node) {
    if(!existsInSet(*set, node)) return ;

    Set *prev = NULL;
    Set* currentElem = set;

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
