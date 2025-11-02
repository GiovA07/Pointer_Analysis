#ifndef SET_H
#define SET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Node Node;
typedef struct Set{
    Node *node;   // Puntero a un arreglo de nodos
    struct Set *next;
} Set;

Set* createSet();

bool set_existElem(Set *set, Node *node);       // Verificar si un nodo esta en el conjunto
bool set_addElem(Set **set, Node *node);        // Agregar un nodo al conjunto
void set_deleteElem(Set **set, Node *node);     // Eliminar un nodo del conjunto
bool set_empty(Set *s);

Set* set_difference(Set *current, Set *old);
Set* set_union(Set *set1, Set *set2);
bool set_union_inplace(Set **dst, Set *src);
Set* set_clone(Set *src);
void set_destroy(Set *s);

void set_print(Set *s);

#endif