#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NODES 10

// Estructura de un nodo del grafo
typedef struct Node {
    char* name;                            // Nombre del nodo (variable)
    struct Set* references;    // Lista de variables a las que apunta
    struct Set* edges;         // Lista de nodos con los que tiene aristas
} Node;

// Funciones para manejar los nodos
Node* createNode(char *name);
void addReference(Node *node, Node *ref);
void removeReference(Node *node, Node *ref);

void addEdgeInNode(Node *node, Node *edge);
void removeEdgeInNode(Node *node, Node *ref);

void printReferences(Node *node);
void printEdges(Node *node);
#endif

