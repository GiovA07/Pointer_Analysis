#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Set.h"

#define MAX_NODES 10

// Estructura de un nodo del grafo
typedef struct Node {
    char* name;
    struct Set* references;    // Pcur(v): el conjunto “actual” de referencias
    struct Set* pold;          // Pold(v): lo que ya hemos enviado en la ultima iteracion
    struct Set* edges;         // aristas del grafo

} Node;

//set-gets
void node_setReferences(Node *node, Set *src);

// Funciones para manejar los nodos
Node* createNode(char *name);
void addReference(Node *node, Node *ref);
void removeReference(Node *node, Node *ref);
void clean_reference(Node *node);
void addEdgeInNode(Node *node, Node *edge);
void removeEdgeInNode(Node *node, Node *ref);
int existEdgeInNode(Node *node, Node *ref);

void printReferences(Node *node);
void printEdges(Node *node);
#endif

