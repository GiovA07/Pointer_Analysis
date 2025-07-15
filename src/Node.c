#include "../include/Node.h"
#include "../include/Set.h"

Node* createNode(char *name) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el nodo.\n");
        exit(EXIT_FAILURE);
    }
    newNode->name = strdup(name);  // Asigna memoria y copia el string

    for (int i = 0; i < MAX_NODES; i++) {
        newNode->references = createSet();
        newNode->edges = createSet();
        newNode->pold = createSet();
    }

    return newNode;
}

void node_setReferences(Node *node, Set *src) {
    Set *oldSet = node->references;
    node->references = src;
    set_destroy(oldSet);
}

// Agrega una referencia a la lista de adyacencia
void addReference(Node *node, Node *ref) {
    // Verificar si ya existe
    if(set_existElem(node->references, ref))
        return ;

    set_addElem(&node->references, ref);
}

void removeReference(Node *node, Node *ref){
    if(!set_existElem(node->references, ref))
        return ;
    
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
    Set *edge = node->edges;
    while (edge) {
        if(edge->node = ref) return 1;
        set_nextElem(edge);
    }
    return 0;
}


// Lista de los nodos a los que apunta.
void printReferences(Node *node) {
    printf("Nodo %s tiene las siguientes referencias:\n", node->name);
    Set *ref = node->references;
    while (ref != NULL) {
        printf("- %s\n", ref->node->name);
        ref = ref->next;
    }
}

// Lista de los nodos que tiene aristas.
void printEdges(Node *node) {
    printf("Nodo %s tiene las siguientes referencias:\n", node->name);
    Set *edge = node->edges;
    while (edge != NULL) {
        printf("- %s\n", edge->node->name);
        edge = edge->next;
    }
}