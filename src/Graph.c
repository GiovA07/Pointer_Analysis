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
        printf("Error: Nodo de origen o destino no valido.\n");
        return;
    }
    addEdgeInNode(from->node, to);
}

void removeEdge(Graph *from, Node* to){
    if (!from || !to) {
        printf("Error: Nodo de origen o destino no valido.\n");
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
        
        Graph *gdst = findNodeResolved(result, n_src->name);
        if (!gdst) continue;
        Node *n_dst = gdst->node;

        // Copiar referencias
        for (Set *r = n_src->references; r; r = r->next) {
            Graph *gr = findNodeResolved(result, r->node->name);
            if (!gr) continue;
            addReference(n_dst, gr->node);
        }
        // Copiar Edges
        for (Set *e = n_src->edges; e; e=e->next) {
            Graph *ge = findNodeResolved(result, e->node->name);
            if (!ge) continue;
            addEdgeInNode(n_dst, ge->node);
        }
        //Copiar alias
        for (Alias *a = n_src->aliases; a; a = a->next)
            node_alias_add(n_dst, a->name);
    }
    return result;
}

Graph* findNodeResolved(Graph *g, char *name) {
    // 1) match directo por name
    for (Graph *cur = g; cur; cur = cur->next)
        if (strcmp(cur->node->name, name) == 0) return cur;

    // 2) match por aliases
    for (Graph *cur = g; cur; cur = cur->next)
        if (node_has_alias(cur->node, name)) return cur;

    return NULL;
}

void unify_node_to_target(Graph *G, Node *target, Node *source) {
    if (!G || !source || !target || source == target) return;
    // 1) Reemplazar edges entrantes y referencias que apuntaban a source
    for (Graph *cur = G; cur; cur = cur->next) {
        Node *x = cur->node;
        if (!x) continue;
        // edges entrantes: x->source  => x->target
        if (set_existElem(x->edges, source)) {
            addEdgeInNode(x, target);
            removeEdgeInNode(x, source);
        }
        // Pcur(x): si apunta a source, redirigir a target
        if (set_existElem(x->references, source)) {
            removeReference(x, source);
            addReference(x, target);
        }
        // Pold(x): si contiene source, redirigir a target
        if (set_existElem(x->pold, source)) {
            set_deleteElem(&x->pold, source);
            set_addElem(&x->pold, target);
        }
    }
}

static void join_unify(Graph **J, Node *target, Node *source) {
    if (!J || !*J || !target || !source || target == source) return;

    unify_node_to_target(*J, target, source);

    // merge sets + edges
    set_union_inplace(&target->references, source->references);
    set_union_inplace(&target->pold,       source->pold);
    for (Set *e = source->edges; e; e = e->next)
        addEdgeInNode(target, e->node);

    removeEdgeInNode(target, target);
    // Merge aliases
    node_alias_merge(target, source);
    //saca a source del join
    removeNode(J, source);
    // libera el nodo de source
    node_destroy(source);
}

static void absorb_name_collision(Graph **J, Node *rep, char *alias_name) {
    Graph *conf = findNodeResolved(*J, alias_name);
    if (conf && conf->node != rep) {
        join_unify(J, rep, conf->node);
    }
}

// Helper: buscar o crear por nombre dentro de un grafo. (UTILIZADO EN EL JOIN) */
static Node* ensure_node_in(Graph **J, Node *node) {
    if (!node) return NULL;

    // 1) busco por name directo
    Graph *g = findNodeResolved(*J, node->name);

    // 2) si no esta, busco por cualquiera de sus aliases
    if (!g) {
        for (Alias *a = node->aliases; a; a = a->next) {
            g = findNodeResolved(*J, a->name);
            if (g) break;
        }
    }

    // 3) si todavia no existe, se crea
    Node *nodeJ;
    if (!g) {
        nodeJ = createNode(node->name);
        addNode(J, nodeJ);
    } else {
        nodeJ = g->node;
    }

    // siempre agregar el name como alias
    node_alias_add(nodeJ, node->name);
    // 4) copiar/mergear todos los aliases del sideNode al representante del join
    for (Alias *a = node->aliases; a; a = a->next) {
        node_alias_add(nodeJ, a->name);
        absorb_name_collision(J, nodeJ, a->name);
    }

    return nodeJ;
}

static void join_merge_side(Graph **J, Graph *side) {
    // asegurar nodos en el join
    for (Graph *g = side; g; g = g->next) {
        ensure_node_in(J, g->node);
    }

    // copiar refs + edges del Grafo al del JOIN
    for (Graph *g = side; g; g = g->next) {
        Node *uJ = ensure_node_in(J, g->node);

        for (Set *r = g->node->references; r; r = r->next) {
            Node *vJ = ensure_node_in(J, r->node);
            addReference(uJ, vJ);
        }
        for (Set *e = g->node->edges; e; e = e->next) {
            Node *vJ = ensure_node_in(J, e->node);
            addEdgeInNode(uJ, vJ);
        }
    }
}

Graph* graph_join(Graph *a, Graph *b) {
    Graph *j = NULL;
    join_merge_side(&j, a);
    join_merge_side(&j, b);

    // TODO: Podriamos ver si hacer la copia de Pold

    return j;
}

int graphs_equal(Graph *a, Graph *b){
    if (a == b) return 1;
    if (!a || !b) return 0;

    // mismo conjunto de nodos por nombre
    for (Graph *ga = a; ga; ga = ga->next) {
        Graph *gb = findNodeResolved(b, ga->node->name);
        if (!gb) return 0;
    }
    for (Graph *gb = b; gb; gb = gb->next) {
        Graph *ga = findNodeResolved(a, gb->node->name);
        if (!ga) return 0;
    }

    // mismas refs, edges y alias por nodo
    for (Graph *ga=a; ga; ga=ga->next){
        Node *na = ga->node;
        Graph *gb = findNodeResolved(b, na->name);
        if (!gb) return 0;
        Node *nb = gb->node;
        
        if (!aliases_equal(na->aliases, nb->aliases)) return 0;
        if(!set_equals(na->references, nb->references)) return 0;
        if(!set_equals(na->edges, nb->edges)) return 0;

    }
    return 1;
}


void graph_destroy(Graph **g) {
    if (!g || !*g) return;

    Graph *cur = *g;
    while (cur) {
        Graph *next = cur->next;

        // el grafo es dueño del Node
        node_destroy(cur->node);

        free(cur);
        cur = next;
    }
    *g = NULL;
}