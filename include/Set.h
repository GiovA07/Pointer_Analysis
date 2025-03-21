#ifndef SET_H
#define SET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Graph.h"
#include "Node.h"

// Estructura para el conjunto C
typedef struct Set{
    Node *node;   // Puntero a un arreglo de nodos
    struct Set *next;
} Set;


Set* createSet();
void addToCSet(Set **set, Node *node); // Agregar un nodo al conjunto
int existsInSet(Set *set, Node *node); // Verificar si un nodo esta en el conjunto
void deleteFromSet(Set **set, Node *node); // Eliminar un nodo del conjunto

#endif