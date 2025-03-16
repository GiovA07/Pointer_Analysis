#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Node.h"
#include "./Graph.h"

// Estructura D (mapea nodos a enteros)
typedef struct DMap {
    Node* node;         
    int value;          // ⊥ representado por -1
    struct DMap* next;
} DMap;

// Estructura R (mapea nodos a otros nodos)
typedef struct RMap {
    Node* node;
    Node* representative;
    struct RMap* next;
} RMap;

DMap* initDMap(Graph* graph);
RMap* initRMap(Graph* graph);
int getDValue(DMap* dMap, Node* node);
int getRValue(RMap* rMap, Node* node);
void setDValue(DMap* dMap, Node* node, int value);
void setRValue(RMap* rMap, Node* node, int value);




#endif