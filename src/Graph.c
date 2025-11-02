#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/Graph.h"


// Busca un nodo en el grafo y devuelve su puntero, o NULL si no existe.
Graph* findNode(Graph *g, char *name) {
    for (; g; g = g->next)
        if (strcmp(g->node->name, name) == 0) return g;
    return NULL;
}

Graph* nextNode(Graph *g){
    //Si no tiene next devolvera null.
    return g->next;
}

// Agrega un nodo al grafo
void addNode(Graph **g, Node *node) {
    if (!g || !node) return;
    if(findNode(*g, node->name)) return;
    Graph *nuevo = (Graph *)malloc(sizeof(Graph));
    if (!nuevo) {
        printf("Error al asignar memoria para el nodo.\n");
        return;
    }
    nuevo->node = node;
    nuevo->next = *g;
    *g = nuevo;
}

void removeNode(Graph **g, Node *node) {
    if (!g || !*g || !node) return;
    Graph *current = *g;
    Graph *prev = NULL;
    while (current) {
        if (current->node == node) {
            // Eliminar el nodo de la lista
            if (prev == NULL)
                // El nodo está al principio de la lista
                *g = current->next;
            else
                prev->next = current->next;
            // Liberar la memoria del nodo
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Agrega una arista entre dos nodos
void addEdge(Graph *from, Node* to) {
    if (!from || !to) {
        printf("Error: Nodo de origen o destino no válido.\n");
        return;
    }
    addEdgeInNode(from->node, to);
}

void removeEdge(Graph *from, Node* to){
    if (!from || !to) {
        printf("Error: Nodo de origen o destino no válido.\n");
        return;
    }
    removeEdgeInNode(from->node, to);
}

// Imprime el grafo en formato de lista de adyacencia
void printGraph(Graph *g) {
    while (g != NULL) {
        printf("Nodo %s: References: \n", g->node->name);
        Set *ref = g->node->references;
        while (ref)
        {
            printf(" - %s\n", ref->node->name);
            ref = ref->next;
        }
        printf("\n");        

        printf("Nodo %s: \n", g->node->name);
        Set *edge = g->node->edges;
        while (edge != NULL) {
           printf(" -> %s\n", edge->node->name);
           edge = edge->next;
        }
        g = g->next;
    }
}

void generateDot(Graph *g, FILE* file) {
    Graph *gOriginal = g;
    while (g) {
        Node* node = g->node;
        char refList[256] = "";

        Set *ref = node->references;
        while(ref != NULL) {
            strcat(refList, ref->node->name);
            if (ref->next != NULL) {
                strcat(refList, ", ");
            }
            ref = ref->next;
        }

        fprintf(file, "%s [label=\"%s {%s}\"];\n", node->name, node->name, refList);
        g = g->next;
    }

    // Ahora, agregar las aristas
    g = gOriginal; 
    while (g) {
        Node* node = g->node;

        Set* edge = node->edges;
        while(edge != NULL) {
            fprintf(file, "%s -> %s;\n", node->name, edge->node->name);
            edge = edge->next;
        }

        g = g->next;
    }
}


void printDot(Graph *g, const char* filename) {
    if (g == NULL) {
        printf("grafo borrado");
        exit(1);
    }
    FILE* file = fopen(filename, "w");
    if (!file) { fprintf(stderr, "Error al abrir %s\n", filename); return; }
    fprintf(file, "digraph{\nrankdir=TB;\n");
    generateDot(g, file);
    fprintf(file, "}\n");
    fclose(file);
}


