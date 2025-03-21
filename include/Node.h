#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 10

// Estructura de un nodo del grafo
typedef struct Node {
    char* name;                            // Nombre del nodo (variable)
    struct Node* references[MAX_NODES];    // Lista de variables a las que apunta
    struct Node* edges[MAX_NODES];         // Lista de nodos con los que tiene aristas
} Node;

// Funciones para manejar los nodos
Node* createNode(char *name);
void addReference(Node *node, Node *ref);
void addEdgeInNode(Node *node, Node *edge);
void printReferences(Node *node);
void printEdges(Node *node);
#endif

