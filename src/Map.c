#include "../include/Map.h"


DMap* initDMap(Graph* graph) {
    DMap *head = NULL;
    Graph *currentGraph = graph;
    while (currentGraph != NULL) {
        DMap *elem = (DMap *)malloc(sizeof(DMap));
        elem->node = currentGraph->node;
        elem->value = UNVISITED;
        elem->next = head;
        head = elem;
        currentGraph = currentGraph->next;
    }
    return head;
}

RMap* initRMap(Graph* graph) {
    RMap *head = NULL;
    Graph *currentGraph = graph;
    while (currentGraph != NULL) {
        RMap *elem = (RMap *)malloc(sizeof(RMap));
        elem->node = currentGraph->node;
        elem->representative = currentGraph->node;          //Inicialmente, cada nodo es su propio representante.
        elem->next = head;
        head = elem;
        currentGraph = currentGraph->next;
    }
    return head;
}


int getDValue(DMap* dMap, Node* node) {
    while (dMap) {
        if (dMap->node == node) {
            return dMap->value;
        }
        dMap = dMap->next;
    }
    return UNVISITED; // Si no se encuentra, retorna ⊥
}

void setDValue(DMap* dMap, Node* node, int value) {
    while (dMap) {
        if (dMap->node == node) {
            dMap->value = value;
            return;
        }
        dMap = dMap->next;
    }
}

Node* getRValue(RMap* rMap, Node* node) {
    while (rMap) {
        if (rMap->node == node) {
            return rMap->representative;
        }
        rMap = rMap->next;
    }
    return NULL;
}


void setRValue(RMap* rMap, Node* node, Node* representative) {
    while (rMap) {
        if (rMap->node == node) {
            rMap->representative = representative;
            return;
        }
        rMap = rMap->next;
    }
}