#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Node.h"
#include "./ListVar.h"

#ifndef GRAPH_H
#define GRAPH_H

#define MAX_GRAPH 50

// Grafo
typedef struct Graph{
    Node* V;                // Nodos actual del grafo
    ListVar* E;        // Lista de nodos adyacentes que tienen aristas con el
    struct Graph* next;
} Graph;

// Funciones para manejar el grafo
Graph* findNode(Graph *g, char *name);
void addNode(Graph **g, Node *node);
void addEdge(Graph *from, Node* to);
void printGraph(Graph *g);
void generateDot(Graph *g, FILE* file);
void printDot(Graph *g, const char* filename);

#endif // GRAPH_H

