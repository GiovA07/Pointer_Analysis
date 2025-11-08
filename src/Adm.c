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
    set_destroy(a->references);         /* borra {&...} en 'a' */
    a->references = NULL;
    removeAllInEdgesTo(g, a);   /* quita todas las x->a */
     /* Pensar si agregar:
       clearPcur(a);
       invalidateCachesFor(a);
    */
}

void constraitBase(Graph **g, char *dst_a, char *src_b) {
    Node *a = ensure_node(g, dst_a);
    Node *b = ensure_node(g, src_b);
    kill_var_state(*g, a);
    addReference(a, b);         /* Agrega: la referencia {b} en el nodo a. */
}


void constraintSimple(Graph **g, char *dst_a, char *src_b) {
    Node *a = ensure_node(g, dst_a);
    Node *b = ensure_node(g, src_b);
    Graph *bGraph = findNode(*g, b->name);
    kill_var_state(*g,a);
    addEdge(bGraph, a);                 /* crea la arista b -> a */
}


// Complex 1: l ⊇ *r
void constraintComplex1(Graph **g, char *l_name, char *r_name) {
    Node *l = ensure_node(g, l_name);
    Node *r = ensure_node(g, r_name);
    addConstraint(&listComplex1, l, r);
    printf("Complex 1 Constraint: %s ⊇ *%s\n", l_name, r_name);
}

// Complex 2: *l ⊇ r
void constraintComplex2 (Graph **g, char *l_name, char *r_name) {
    Node *l = ensure_node(g, l_name);
    Node *r = ensure_node(g, r_name);
    addConstraint(&listComplex2, l, r);
    printf("Complex 2 Constraint: *%s ⊇ %s\n", l_name, r_name);
}
