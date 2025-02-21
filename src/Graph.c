#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/Graph.h"


// Busca un nodo en el grafo y devuelve su puntero, o NULL si no existe.
Graph* findNode(Graph *g, char *name) {
    while (g != NULL) {
        if (strcmp(g->V->name, name) == 0) {
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
    nuevo->V = node;
    nuevo->E = NULL;
    nuevo->next = *g;
    *g = nuevo;
}

// Agrega una arista entre dos nodos
void addEdge(Graph *from, Node* to) {
    if (!from || !to) {
        printf("Error: Nodo de origen o destino no válido.\n");
        return;
    }

    // Crear nueva conexión en la lista de adyacencia
    ListVar *newList = (ListVar *)malloc(sizeof(ListVar));
    if (!newList) {
        printf("Error al asignar memoria para la arista.\n");
        return;
    }
    newList->var = to;
    newList->next = from->E;
    from->E = newList;
}

// Imprime el grafo en formato de lista de adyacencia
void printGraph(Graph *g) {
    while (g != NULL) {
        printf("Nodo %s: \n", g->V->name);
        ListVar *temp = g->E;
        while (temp != NULL) {
            printf("Nodo %s -> %s \n", g->V->name, temp->var->name);
            temp = temp->next;
        }
        g = g->next;
    }
}

void generateDot(Graph *g, FILE* file) {
    Graph *gOriginal = g;
    while (g) {
        Node* node = g->V;
        struct Node** listRef = g->V->references;
        char refList[256] = "";

        struct Node** current = listRef;
        int i = 0;
        while (current[i] != NULL) {
            strcat(refList, current[i]->name);
            if (current[i+1] != NULL) {
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
        Node* node = g->V;
        ListVar* listRef = g->E;

        ListVar* current = listRef;
        while (current) {
            fprintf(file, "%s -> %s;\n", node->name, current->var->name);
            current = current->next;
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
