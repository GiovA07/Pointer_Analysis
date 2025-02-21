#include "../include/ListVar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Inicializa la lista
ListVar* initList() {
    return NULL;
}

// Agrega un nodo a la lista si no existe
void addVar(ListVar **head, Node *node) {
    if (getVar(*head, node->name) != NULL) {
        return;
    }

    ListVar *newVar = (ListVar *)malloc(sizeof(ListVar));
    if (!newVar) {
        fprintf(stderr, "Error: No se pudo asignar memoria para la variable.\n");
        exit(EXIT_FAILURE);
    }

    newVar->var = node;
    newVar->next = *head;
    *head = newVar;
}

// Busca un nodo en la lista por nombre
Node *getVar(ListVar *head, char *name) {
    ListVar *current = head;
    while (current) {
        if (strcmp(current->var->name, name) == 0) {
            return current->var;
        }
        current = current->next;
    }
    return NULL; // No encontrado
}

// Imprime la lista de variables/nodos
void printList(ListVar *head) {
    ListVar *current = head;
    while (current) {
        printReferences(current->var);

        current = current->next;
    }
}
