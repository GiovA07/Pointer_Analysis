#include "../include/Map.h"


DMap* initDMap(Graph* graph) {
    DMap *head = NULL;
    for (Graph *g = graph; g; g = g->next) {
        DMap *e = (DMap*)malloc(sizeof(DMap));
        if (!e) { 
            destroyDMap(&head); 
            return NULL; 
        }
        e->node  = g->node;
        e->value = UNVISITED;
        e->next  = head;
        head = e;
    }
    return head;
}

RMap* initRMap(Graph* graph) {
    RMap *head = NULL;
    for (Graph *g = graph; g; g = g->next) {
        RMap *elem = (RMap*)malloc(sizeof(RMap));
        if (!elem) { 
            destroyRMap(&head); 
            return NULL; 
        }
        elem->node = g->node;
        elem->representative = g->node;   // cada nodo es su propio rep
        elem->next = head;
        head = elem;
    }
    return head;
}

void destroyDMap(DMap **pd) {
    if (!pd) return;
    DMap *d = *pd;
    while (d) { 
        DMap* nxt = d->next; 
        free(d); 
        d = nxt; 
    }
    *pd = NULL;
}

void destroyRMap(RMap **pr) {
    if (!pr) return;
    RMap *r = *pr;
    while (r) { 
        RMap* nxt = r->next;
        free(r);
        r = nxt; 
    }
    *pr = NULL;
}

int getDValue(DMap* dMap, Node* node) {
    for (DMap* d = dMap; d; d = d->next)
        if (d->node == node) return d->value;
    return UNVISITED; // Si no se encuentra, retorna ⊥
}

void setDValue(DMap* dMap, Node* node, int value) {
    for (DMap* d = dMap; d; d = d->next) {
        if (d->node == node) {
            d->value = value;
            return;
        }
    }
}

Node* getRValue(RMap* rMap, Node* node) {
    for (RMap* r = rMap; r; r = r->next)
        if (r->node == node) 
            return r->representative;
    return NULL;
}



void setRValue(RMap* rMap, Node* node, Node* representative) {
    for (RMap* r = rMap; r; r = r->next) {
        if (r->node == node) {
            r->representative = representative;
            return;
        }
    }
}