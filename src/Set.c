#include "../include/Set.h"
#include "../include/Node.h" 

Set* createSet(){ return NULL; }

bool set_existElem(Set *set, Node *node) {
    for (Set *cur = set; cur; cur = cur->next)
        if (cur->node == node) return true;
    return false;
}

bool set_addElem(Set **set, Node *node) {
    if (!set || !node) return false;
    if (set_existElem(*set, node)) return false;

    Set *newElem = (Set*)malloc(sizeof(Set));
    if (!newElem) return false;
    newElem->node = node;
    newElem->next = *set;
    *set = newElem;
    return true;
}

void set_deleteElem(Set **set, Node *node) {
    if (!set || !*set) return;

    Set *prev = NULL;
    Set* cur = *set;

    while (cur) {
        if(cur->node == node) {
            if (prev == NULL)
                *set = cur->next;
            else
                prev->next = cur->next;
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
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

    // agregamos todos los elementos de set1 y set2
    for (Set *c = set1; c; c = c->next)
        set_addElem(&result, c->node);

    for (Set *c = set2; c; c = c->next)
        set_addElem(&result, c->node);

    return result;
}

// Devuelve true si agrego al menos un elemento de src en *dst.
bool set_union_inplace(Set **dst, Set *src) {
    bool changed = false;
    for (Set *setCur = src; setCur; setCur = setCur->next) {
        if (!set_existElem(*dst, setCur->node)) {
            set_addElem(dst, setCur->node);
            changed = true;
        }
    }
    return changed;
}

Set* set_clone(Set *src) {
    Set *clone = NULL;
    for (Set *c = src; c; c = c->next)
        set_addElem(&clone, c->node);
    return clone;
}

void set_destroy(Set **s) {
    if (!s) return;
    Set *s_cur = *s;
    while (s_cur) {
        Set *next = s_cur->next;
        free(s_cur);
        s_cur = next;
    }
    *s = NULL;
}

bool set_empty(Set *s) { return s == NULL; }

void set_print(Set *set) {
    for (; set; set = set->next)
        printf("- %s\n", set->node->name);
}


static int set_contains_name(Set *s, char *name) {
    for (; s; s = s->next) 
        if (strcmp(s->node->name, name) == 0)  {
            return 1;
        }
    return 0;
}

bool set_equals(Set *a, Set *b) {
    if (a == b) return true; 
    if (!a || !b) return false; 

   // a ⊆ b
    for (Set *p = a; p; p = p->next)
        if (!set_contains_name(b, p->node->name)) return false;

    // b ⊆ a
    for (Set *q = b; q; q = q->next)
        if (!set_contains_name(a, q->node->name)) return false;

    return true;
}