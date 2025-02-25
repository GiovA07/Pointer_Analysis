#ifndef LISTVAR_H
#define LISTVAR_H

#include "node.h"

typedef struct ListVar {
    Node *var;                  // Apunta a la "variable/nodo" real
    struct ListVar *next;       // Siguiente variable/nodo en la lista global
} ListVar;

// Funciones para manejar la lista de variables/nodos global
ListVar* initList();
void addVar(ListVar **head, Node *node);
Node *getVar(ListVar *head, char *name);
void printList(ListVar *head);

#endif // LISTNODE_H
