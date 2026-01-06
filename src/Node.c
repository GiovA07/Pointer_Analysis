#include "../include/Node.h"
#include "../include/Set.h"

Node* createNode(char *name) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (!newNode) {
        fprintf(stderr, "Error: No se pudo asignar memoria para el nodo.\n");
        exit(EXIT_FAILURE);
    }
    newNode->name       = strdup(name);   // copia el string
    newNode->references = createSet();    // Pcur
    newNode->edges      = createSet();    // adyacencia
    newNode->pold       = createSet();    // Pold
    newNode->aliases    = NULL;
    node_alias_add(newNode, name);
    return newNode;
}

void node_setReferences(Node *node, Set *src) {
    Set *oldSet = node->references;
    node->references = src;
    set_destroy(oldSet);
}

/* Agrega una referencia (points-to) si no existe */
void addReference(Node *node, Node *ref) {
    // Verificar si ya existe
    set_addElem(&node->references, ref); // set_addElem ya evita duplicados
}

void removeReference(Node *node, Node *ref){
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
    return set_existElem(node->edges, ref) ? 1 : 0;
}



static int alias_contains(Alias *a, char *name){
    for(; a; a=a->next) 
        if(strcmp(a->name,name)==0) return 1;
    return 0;
}

void node_alias_add(Node *n, char *name){
    if(!n || !name) return;
    if(alias_contains(n->aliases, name)) return;

    Alias *x = malloc(sizeof(Alias));
    x->name = strdup(name);
    x->next = n->aliases;
    n->aliases = x;
}

/*TODO: ver si aca puedo no eliminar el alias, 
    ya que despues hago un destroy del nodo source y eso lo elimina*/
void node_alias_merge(Node *target, Node *source){
    if(!target || !source || target==source) return;

    // el nombre principal del source pasa a ser alias
    node_alias_add(target, source->name);

    // mover aliases
    Alias *a = source->aliases;
    while(a){
        node_alias_add(target, a->name);
        Alias *nx = a->next;
        free(a->name);
        free(a);
        a = nx;
    }
    source->aliases = NULL;
}

int node_has_alias(Node *n, char *name){
    if(!n || !name) return 0;
    if(strcmp(n->name, name)==0) return 1;
    return alias_contains(n->aliases, name);
}

int aliases_equal(Alias *a, Alias *b) {
    if (a == b) return 1;
    // a ⊆ b
    for (Alias *p = a; p; p = p->next)
        if (!alias_contains(b, p->name)) return 0;

    // b ⊆ a
    for (Alias *q = b; q; q = q->next)
        if (!alias_contains(a, q->name)) return 0;

    return 1;
}


 static int alias_count(Alias *a){
    int k=0;
    for(; a; a=a->next) k++;
    return k;
}

int node_isalias_grouped(Node *n){
    // grouped si tiene algún alias distinto de su name
    if(!n) return 0;
    int cnt = alias_count(n->aliases);
    // si aliases incluye el name, entonces el grupo deberia tiener >=2
    return cnt >= 2;
}

void node_alias_remove(Node *n, char *name){
    if(!n || !name) return;
    Alias *prev=NULL, *cur=n->aliases;
    while(cur){
        if(strcmp(cur->name,name)==0){
            if(!prev) n->aliases = cur->next;
            else prev->next = cur->next;
            free(cur->name);
            free(cur);
            return;
        }
        prev=cur;
        cur=cur->next;
    }
}

// Lista de los nodos a los que apunta.
void printReferences(Node *node) {
    printf("Nodo %s tiene las siguientes referencias:\n", node->name);
    Set *ref = node->references;
    while (ref) {
        printf("- %s\n", ref->node->name);
        ref = ref->next;
    }
}

// Lista de los nodos que tiene aristas.
void printEdges(Node *node) {
    printf("Nodo %s tiene las siguientes referencias:\n", node->name);
    Set *edge = node->edges;
    while (edge) {
        printf("- %s\n", edge->node->name);
        edge = edge->next;
    }
}

void node_destroy(Node *n) {
    if (!n) return;

    free(n->name);

    set_destroy(n->references);
    set_destroy(n->edges);
    set_destroy(n->pold);

    Alias *a = n->aliases;
    while (a) {
        Alias *nx = a->next;
        free(a->name);
        free(a);
        a = nx;
    }
    free(n);
}
