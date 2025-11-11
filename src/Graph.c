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



Graph* graph_clone(Graph *src) {
    if (!src) return NULL;
    Graph *result = NULL;

    // 1) clonar nodos
    for (Graph *g=src; g; g=g->next) {
        addNode(&result, createNode(g->node->name));
    }

    // 2) copiar references y edges por nombre
    for (Graph *g=src; g; g=g->next) {
        Node *n_src = g->node;
        Node *n_dst = findNode(result, n_src->name)->node;

        for (Set *r=n_src->references; r; r=r->next) {
            Node *v_dst = findNode(result, r->node->name)->node;
            addReference(n_dst, v_dst);
        }
        for (Set *e=n_src->edges; e; e=e->next) {
            Node *v_dst = findNode(result, e->node->name)->node;
            addEdgeInNode(n_dst, v_dst);
        }
    }
    return result;
}

// Helper: buscar o crear por nombre dentro de un grafo. (UTILIZADO EN EL JOIN) */
static Node* ensure_node_in(Graph **J, char *name) {
    Graph *g = findNode(*J, name);
    if (g) return g->node;
    Node *n = createNode(name);
    addNode(J, n);
    return n;
}

Graph* graph_join(Graph *a, Graph *b) {
    Graph *j = NULL;

    // 1) Asegurar todos los nodos de A y B en el join
    for (Graph *ga = a; ga; ga = ga->next) {
        ensure_node_in(&j, ga->node->name);
    }
    for (Graph *gb = b; gb; gb = gb->next) {
        ensure_node_in(&j, gb->node->name);
    }

    // 2) Copiar referencias y aristas de A al grafo de join
    for (Graph *ga = a; ga; ga = ga->next) {
        Node *na_j = ensure_node_in(&j, ga->node->name);
        /* referencias (Pcur) */
        for (Set *r = ga->node->references; r; r = r->next) {
            Node *v_j = ensure_node_in(&j, r->node->name);
            addReference(na_j, v_j);
        }
        /* aristas */
        for (Set *e = ga->node->edges; e; e = e->next) {
            Node *v_j = ensure_node_in(&j, e->node->name);
            addEdgeInNode(na_j, v_j);
        }
    }

    // 3) Copiar referencias y aristas de B al grafo de join
    for (Graph *gb = b; gb; gb = gb->next) {
        Node *nb_j = ensure_node_in(&j, gb->node->name);
        for (Set *r = gb->node->references; r; r = r->next) {
            Node *v_j = ensure_node_in(&j, r->node->name);
            addReference(nb_j, v_j);
        }
        for (Set *e = gb->node->edges; e; e = e->next) {
            Node *v_j = ensure_node_in(&j, e->node->name);
            addEdgeInNode(nb_j, v_j);
        }
    }

    // 4) ver si hacer el Pold(j) := Pcur(j) para arrancar limpio el proximo WP

    return j;
}

static int sets_same(Set *a, Set *b){
    // igualdad por identidad de nodos (punteros) — tus sets guardan Node*
    for (Set *x=a; x; x=x->next) if (!set_existElem(b, x->node)) return 0;
    for (Set *y=b; y; y=y->next) if (!set_existElem(a, y->node)) return 0;
    return 1;
}

static Node* find_by_name(Graph *g, char *name) {
    Graph *gname = findNode(g, name);
    if(gname->node) return gname->node;
    return NULL;
}

int graphs_equal(Graph *a, Graph *b){
    // mismo conjunto de nodos por nombre
    for (Graph *ga = a; ga; ga = ga->next){
        Node *nName = find_by_name(b, ga->node->name);
        if (!nName) return 0;
    }
    for (Graph *gb=b; gb; gb=gb->next){
        Node *nName = find_by_name(a, gb->node->name);
        if (!nName) return 0;
    }
    // mismas refs y edges por nodo
    for (Graph *ga=a; ga; ga=ga->next){
        Node *na = ga->node;
        Node *nb = find_by_name(b, na->name);
        if (!nb) return 0;
        if (!sets_same(na->references, nb->references)) return 0;
        if (!sets_same(na->edges,      nb->edges))      return 0;
    }
    return 1;
}
