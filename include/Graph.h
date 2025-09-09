#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Node.h"


// Grafo
typedef struct Graph{
    Node* node;                // Nodo actual del grafo
    struct Graph* next;        // Siguiente nodo
} Graph;

// Funciones para manejar el grafo
Graph* findNode(Graph *g, char *name);
Graph* nextNode(Graph *g);

void addNode(Graph **g, Node *node);
void removeNode(Graph **g, Node *node);
void addEdge(Graph *from, Node* to);
void removeEdge(Graph *from, Node* to);

void printGraph(Graph *g);
void generateDot(Graph *g, FILE* file);
void printDot(Graph *g, const char* filename);

#endif // GRAPH_H
