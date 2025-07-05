#include "../include/Set.h"

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
    if(!current || !old) {
        fprintf(stderr, "Error: sets not valid to make their difference.\n");
        exit(EXIT_FAILURE);
    }
    Set *result = createSet();

    while(current != NULL) {
        if(!set_existElem(old, current->node)) {
            set_addElem(&result, current->node);
        }
        current = set_nextElem(current);
    }
}
