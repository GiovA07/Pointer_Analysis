#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Set.h"

#define MAX_NODES 10

/* MACROS */
#define Pcur(n) ((n)->references)
#define Pold(n) ((n)->pold)

typedef struct Alias {
    char *name;
    struct Alias *next;
} Alias;

// Estructura de un nodo del grafo
typedef struct Node {
    char* name;
    Set* references;    // Pcur(v): el conjunto “actual” de referencias
    Set* pold;          // Pold(v): lo que ya hemos enviado en la ultima iteracion
    Set* edges;         // aristas del grafo
    
    Alias *aliases;
} Node;

//set-gets
void node_setReferences(Node *node, Set *src);

// Funciones para manejar los nodos
Node* createNode(char *name);

void addReference(Node *node, Node *ref);
void removeReference(Node *node, Node *ref);

void addEdgeInNode(Node *node, Node *edge);
void removeEdgeInNode(Node *node, Node *ref);
int existEdgeInNode(Node *node, Node *ref);

int  node_has_alias(Node *n, char *name);
void node_alias_add(Node *n, char *name);
void node_alias_merge(Node *target, Node *source);

int aliases_equal(Alias *a, Alias *b);

int  node_isalias_grouped(Node *n);
void node_alias_remove(Node *n, char *name);

void printReferences(Node *node);
void printEdges(Node *node);

void node_destroy(Node *n);
#endif