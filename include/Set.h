#ifndef SET_H
#define SET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node Node;

// Estructura para el conjunto C
typedef struct Set{
    Node *node;   // Puntero a un arreglo de nodos
    struct Set *next;
} Set;


Set* createSet();

Set* set_nextElem(Set *set);

void set_addElem(Set **set, Node *node); // Agregar un nodo al conjunto
int set_existElem(Set *set, Node *node); // Verificar si un nodo esta en el conjunto
void set_deleteElem(Set **set, Node *node); // Eliminar un nodo del conjunto

Set* set_difference(Set *current, Set *old);
Set* set_union(Set *set1, Set *set2);
Set* set_clone(Set *src);

void set_destroy(Set *s);

void set_print(Set *s);

#endif