#include "../include/Node.h"
#include "../include/Set.h"

Node* createNode(char *name) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el nodo.\n");
        exit(EXIT_FAILURE);
    }
    newNode->name       = strdup(name);   // copia el string (necesita <string.h>)
    newNode->references = createSet();    // Pcur
    newNode->edges      = createSet();    // adyacencia
    newNode->pold       = createSet();    // Pold
    return newNode;
}

void node_setReferences(Node *node, Set *src) {
    Set *oldSet = node->references;
    node->references = src;
    set_destroy(oldSet);
}

/* Agrega una referencia (points-to) si no existe */
void addReference(Node *node, Node *ref) {
    // Verificar si ya existe
    set_addElem(&node->references, ref); // set_addElem ya evita duplicados
}

void clean_reference(Node *node) {
    Set *ref = node->references;
    while(ref) {
        Set *aux = ref;
        ref = ref->next;
        free(aux);
    }
    node->references = NULL;
}

void removeReference(Node *node, Node *ref){
    set_deleteElem(&node->references, ref);
}

void addEdgeInNode(Node *node, Node *edge) {
    Set **edges = &node->edges;
    if (set_existElem(*edges, edge)) return ;

    set_addElem(edges, edge);
}

void removeEdgeInNode(Node *node, Node *edge) {
    Set **edges = &node->edges;
    if (!set_existElem(*edges, edge)) return ;

    set_deleteElem(edges, edge);
}

int existEdgeInNode(Node *node, Node *ref) {
    return set_existElem(node->edges, ref) ? 1 : 0;
}

// Lista de los nodos a los que apunta.
void printReferences(Node *node) {
    printf("Nodo %s tiene las siguientes referencias:\n", node->name);
    Set *ref = node->references;
    while (ref) {
        printf("- %s\n", ref->node->name);
        ref = ref->next;
    }
}

// Lista de los nodos que tiene aristas.
void printEdges(Node *node) {
    printf("Nodo %s tiene las siguientes referencias:\n", node->name);
    Set *edge = node->edges;
    while (edge) {
        printf("- %s\n", edge->node->name);
        edge = edge->next;
    }
}