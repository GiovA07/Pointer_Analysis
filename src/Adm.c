#include "../include/Adm.h"

/* 
* Busca el nodo por nombre; si no existe, lo crea y lo agrega al grafo. 
* Si existe (por name o alias), devolvemos el representante y registramos 'name' como alias.
*/
static Node* ensure_node(Graph **g, char *name) {
    Graph *existing = findNode(*g, name);
    if (existing) {
        node_alias_add(existing->node, name);
        return existing->node;
    } 
    Node *n = createNode(name);
    addNode(g, n);
    return n;
}

/* Elimina TODAS las aristas entrantes (x -> a) el grafo */
static void removeAllInEdgesTo(Graph *g, Node *a) {
    for (Graph *cur = g; cur; cur = cur->next) {
        Node *x = cur->node;
        node_removeEdge(x, a);    /* quita x->a si existe */
    }
}

/*  
 * Strong Update: Destruye el estado previo de la variable 'a'.
 */
static void kill_var_state(Graph *g, Node *a) {
    set_destroy(&a->references);    /* Borra {&...} en 'a' */
    a->references = createSet();
    
    removeAllInEdgesTo(g, a);       /* Borra aristas entrantes x->a */
    
    set_destroy(&a->edges);         /* Borra aristas salientes a -> x*/
    a->edges = createSet();
    
    set_destroy(&Pold(a));
    Pold(a) = NULL;
}

/* Busca un nombre de reemplazo dentro de la lista de alias de un nodo */
/* Devuelve el primer nombre de la lista que no sea igual al excluido */
static char* pick_other_alias(Node *n, char *exclude) {
    for (Alias *a = n->aliases; a; a = a->next) {
        if (strcmp(a->name, exclude) != 0) return a->name;
    }
    return NULL;
}

/* Separa un alias de su grupo y lo convierte en un nodo independiente. */
static Node* delete_group_alias(Graph **g, Node *node, char *name) {
    if (!g || !node || !name) return node;

    // Verifica si el nodo tiene un grupo de alias.
    if (!node_isalias_grouped(node)) return node;

    // Verifica si el nombre a quitar pertenece al alias del nodo.
    if (!node_has_alias(node, name)) return node;

    // 1) Borra name de la lista de alias del nodo
    node_alias_remove(node, name);
    // 2) Si el name principal era justo el que sacamos, reasignamos otro del mismo grupo
    if (strcmp(node->name, name) == 0) {
        char *new_main = pick_other_alias(node, name);
        if (new_main) {
            free(node->name);
            node->name = strdup(new_main);
        }
    }
    // 3) Crear nodo para el 'name' y meterlo al grafo
    Node *new_node = createNode((char*)name);
    addNode(g, new_node);

    return new_node;
}

// Base Constraint: a = &b  (a ⊇ {b})
void constraitBase(Graph **g, char *dst_a, char *src_b) {
    Node *a = ensure_node(g, dst_a);
    a = delete_group_alias(g, a, dst_a);
    Node *b = ensure_node(g, src_b);
    kill_var_state(*g, a);
    addReference(a, b);         /* Agrega la referencia {b} en el nodo a. */
    printf("[Operator] Base: %s ⊇ %s\n", dst_a, src_b);
}

// Simple Constraint: a = b  (a ⊇ b)
void constraintSimple(Graph **g, char *dst_a, char *src_b) {
    Node *a = ensure_node(g, dst_a);
    a = delete_group_alias(g, a, dst_a);
    Node *b = ensure_node(g, src_b);

    kill_var_state(*g,a);    // Strong update sobre a

    Graph *bGraph = findNode(*g, b->name);
    if (!bGraph) return;
    addEdge(bGraph, a);                 /* crea la arista b -> a */

    printf("[Operator] Simple: %s ⊇ %s\n", dst_a, src_b);
}

// Complex 1 Constraint (LOAD): l = *r  (l ⊇ *r)
void constraintComplex1(Graph **g, char *l_name, char *r_name) {
    Node *l = ensure_node(g, l_name);
    l = delete_group_alias(g, l, l_name);
    Node *r = ensure_node(g, r_name);

    kill_var_state(*g, l);  // Strong update sobre l
    addConstraint(&listComplex1, l_name, r_name);
    printf("[Operator] Complex 1 Constraint: %s ⊇ *%s\n", l_name, r_name);
}

/* Funcion auxiliar, indica si el conjunto Pcur del nodo tiene exactamente un elemento (Singleton)*/
static Node* pcur_unique_target(Node *l) {
    Set *pts = Pcur(l);
    if (!pts) return NULL;
    if (pts->next) return NULL;
    return pts->node;                // Retorna el unico objetivo
}

// Complex 2 Constraint (STORE): *l = r  (*l ⊇ r)
void constraintComplex2(Graph **g, char *l_name, char *r_name) {
    Node *l = ensure_node(g, l_name);
    Node *r = ensure_node(g, r_name);
    /* Si el puntero 'l' apunta exactamente a un solo lugar (Singleton),
     * podemos hacer un strong update sobre el. */
    Node *n = pcur_unique_target(l);
    if (n) {
        kill_var_state(*g, n);
    }

    addConstraint(&listComplex2, l_name, r_name);
    printf("[Operator] Complex 2 Constraint: *%s ⊇ %s\n", l_name, r_name);
}