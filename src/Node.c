#include "../include/Node.h"

Node* createNode(char *name) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el nodo.\n");
        exit(EXIT_FAILURE);
    }
    newNode->name = strdup(name);  // Asigna memoria y copia el string

    for (int i = 0; i < MAX_NODES; i++) {
        newNode->references[i] = NULL;
    }

    return newNode;
}

// Agrega una referencia a la lista de adyacencia
void addReference(Node *node, Node *ref) {
    // Buscar un espacio vacío en el arreglo de referencias
    for (int i = 0; i < MAX_NODES; i++) {
        if (node->references[i] == NULL) {
            node->references[i] = ref;
            return;
        }
    }
}

// Lista de los nodos a los que apunta.
void printReferences(Node *node) {
    printf("Nodo %s tiene las siguientes referencias:\n", node->name);
    for (int i = 0; i < 10; i++) {
        if (node->references[i] != NULL) {
            printf("- %s\n", node->references[i]->name);
        }
    }
}