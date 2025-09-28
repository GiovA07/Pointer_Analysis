#include "../include/Adm.h"


/* Busca el nodo por nombre; si no existe, lo crea y lo agrega al grafo. */
static Node* ensure_node(Graph **g, char *name) {
    Graph *aux = findNode(*g, name);
    if (aux) return aux->node;
    Node *n = createNode(name);
    addNode(g, n);
    return n;
}

/* Elimina TODAS las aristas entrantes x -> a barriendo el grafo */
static void removeAllInEdgesTo(Graph *g, Node *a) {
    for (Graph *cur = g; cur; cur = cur->next) {
        Node *x = cur->node;
        removeEdgeInNode(x, a);    /* quita x->a si existe */
    }
}

/*  Mata el estado previo de la variable 'a' */
static void kill_var_state(Graph *g, Node *a) {
    clean_reference(a);         /* borra {&...} en 'a' */
    removeAllInEdgesTo(g, a);   /* quita todas las x->a */
     /* Pensar si agregar:
       clearPcur(a);
       invalidateCachesFor(a);
    */
}

void constraitBaseAlg(Graph **g, char *dst_a, char *src_b) {
    Node *a = ensure_node(g, dst_a);
    Node *b = ensure_node(g, src_b);
    // kill_var_state(*g, a);
    addReference(a, b);         /* Agrega: la referencia {b} en el nodo a. */
}


void constraintSimpleAlg(Graph **g, char *dst_a, char *src_b) {
    Node *a = ensure_node(g, dst_a);
    Node *b = ensure_node(g, src_b);
    Graph *bGraph = findNode(*g, b->name);
    // kill_var_state(*g,a);
    addEdge(bGraph, a);                 /* crea la arista b -> a */
}
