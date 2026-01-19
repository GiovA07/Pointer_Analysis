#include "../include/Adm.h"

/* Busca el nodo por nombre; si no existe, lo crea y lo agrega al grafo. */
// Si existe (por name o alias), devolvemos el representante y registramos 'name' como alias.
static Node* ensure_node(Graph **g, char *name) {
    Graph *existing = findNodeResolved(*g, name);
    if (existing) {
        node_alias_add(existing->node, name);
        return existing->node;
    } 
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
    set_destroy(&a->references);         /* borra {&...} en 'a' */
    a->references = createSet();
    
    removeAllInEdgesTo(g, a);           /* borra aristas salientes x->a */
    
    set_destroy(&a->edges);             /* borra aristas entrantes a -> x*/
    a->edges = createSet();
    
    set_destroy(&Pold(a));
    Pold(a) = NULL;
}

static char* pick_other_alias(Node *n, char *exclude) {
    for (Alias *a = n->aliases; a; a = a->next) {
        if (strcmp(a->name, exclude) != 0) return a->name;
    }
    return NULL;
}


static Node* delete_group_alias(Graph **g, Node *node, char *name) {
    if (!g || !node || !name) return node;

    // Si no está realmente agrupado, usar el mismo nodo.
    if (!node_isalias_grouped(node)) return node;

    // Si 'name' no pertenece al grupo (defensivo), no hagas nada raro.
    if (!node_has_alias(node, name)) return node;

    // 1) sacamos name del grupo
    node_alias_remove(node, name);
    // 2) si el name principal era justo el que sacamos, elegimos otro
    if (strcmp(node->name, name) == 0) {
        char *new_main = pick_other_alias(node, name);
        if (new_main) {
            free(node->name);
            node->name = strdup(new_main);
        }
    }
    // 3) crear nodo para el 'name' y meterlo al grafo
    Node *new_node = createNode((char*)name);
    addNode(g, new_node);

    return new_node;
}


void constraitBase(Graph **g, char *dst_a, char *src_b) {
    Node *a = ensure_node(g, dst_a);
    a = delete_group_alias(g, a, dst_a);
    Node *b = ensure_node(g, src_b);
    kill_var_state(*g, a);
    addReference(a, b);         /* Agrega la referencia {b} en el nodo a. */
    printf("[Operator] Base: %s ⊇ %s\n", dst_a, src_b);
}


void constraintSimple(Graph **g, char *dst_a, char *src_b) {
    Node *a = ensure_node(g, dst_a);
    a = delete_group_alias(g, a, dst_a);
    Node *b = ensure_node(g, src_b);

    kill_var_state(*g,a);

    Graph *bGraph = findNodeResolved(*g, b->name);
    if (!bGraph) return;
    addEdge(bGraph, a);                 /* crea la arista b -> a */
    // set_union_inplace(&Pcur(a), Pold(b)); 
    printf("[Operator] Simple: %s ⊇ %s\n", dst_a, src_b);
}

// Complex 1: l ⊇ *r (l = *r)
void constraintComplex1(Graph **g, char *l_name, char *r_name) {
    Node *l = ensure_node(g, l_name);
    l = delete_group_alias(g, l, l_name);
    Node *r = ensure_node(g, r_name);

    kill_var_state(*g, l);
    addConstraint(&listComplex1, l_name, r_name);
    printf("[Operator] Complex 1 Constraint: %s ⊇ *%s\n", l_name, r_name);
}

static Node* pcur_unique_target(Node *l) {
    Set *pts = Pcur(l);
    if (!pts) return NULL;
    if (pts->next) return NULL;
    return pts->node;                // exactamente 1 solo
}

// Complex 2: *l ⊇ r  (*l = r)
void constraintComplex2(Graph **g, char *l_name, char *r_name) {
    Node *l = ensure_node(g, l_name);
    Node *r = ensure_node(g, r_name);

    Node *n = pcur_unique_target(l);
    if (n) { //Kill sobre el nodo target
        kill_var_state(*g, n);
    }

    addConstraint(&listComplex2, l_name, r_name);
    printf("[Operator] Complex 2 Constraint: *%s ⊇ %s\n", l_name, r_name);
}