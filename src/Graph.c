#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/Graph.h"


// Busca un nodo en el grafo y devuelve su puntero, o NULL si no existe.
Graph* findNode(Graph *g, char *name) {
    while (g != NULL) {
        if (strcmp(g->node->name, name) == 0) {
            return g;
        }
        g = g->next;
    }
    return NULL; // No encontrado
}

// Agrega un nodo al grafo
void addNode(Graph **g, Node *node) {
    
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

// Agrega una arista entre dos nodos
void addEdge(Graph *from, Node* to) {
    if (!from || !to) {
        printf("Error: Nodo de origen o destino no válido.\n");
        return;
    }
    addEdgeInNode(from->node, to);
}

// Imprime el grafo en formato de lista de adyacencia
void printGraph(Graph *g) {
    while (g != NULL) {
        printf("Nodo %s: \n", g->node->name);
        for (int i = 0; i < MAX_NODES; i++) {
            if (g->node->edges[i] != NULL) {
                printf(" -> %s\n", g->node->edges[i]->name);
            }
        }
        g = g->next;
    }
}

void generateDot(Graph *g, FILE* file) {
    Graph *gOriginal = g;
    while (g) {
        Node* node = g->node;
        char refList[256] = "";

        // Concatenar las referencias
        int i = 0;
        while (node->references[i] != NULL && i < MAX_NODES) {
            strcat(refList, node->references[i]->name);
            if (node->references[i+1] != NULL) {
                strcat(refList, ", ");
            }
            i++;
        }

        

        fprintf(file, "%s [label=\"%s {%s}\"];\n", node->name, node->name, refList);
        g = g->next;
    }

    // Ahora, agregar las aristas
    g = gOriginal; 
    while (g) {
        Node* node = g->node;
        for (int i = 0; i < MAX_NODES; i++) {
            if (node->edges[i] != NULL) {
                fprintf(file, "%s -> %s;\n", node->name, node->edges[i]->name);
            }
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
    if (file == NULL) {
        fprintf(stderr, "Error al abrir el archivo %s\n", filename);
        return;
    }
    fprintf(file, "digraph{\n\n");
    fprintf(file, "rankdir=TB;\n\n");
    printf("aca llega");
    generateDot(g, file);
    fprintf(file, "}\n");
    fclose(file);
}
