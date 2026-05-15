
#include "../include/Algorithms.h"



ListConstraint *listComplex1 = NULL;  // a ⊇ *b
ListConstraint *listComplex2 = NULL;  // *a ⊇ b

static void wpctx_init(WPContext *ctx, Graph *G) {
    memset(ctx, 0, sizeof(*ctx));
    ctx->D = initDMap(G);
    ctx->R = initRMap(G);
    ctx->S = stack_create();
    ctx->T = stack_create();
    ctx->C = createSet();
    ctx->I = 0;
}

static void wpctx_destroy(WPContext *ctx) {
    destroyDMap(&ctx->D);
    destroyRMap(&ctx->R);
    set_destroy(&ctx->C);
    stack_destroy(&ctx->S);
    stack_destroy(&ctx->T);
    memset(ctx, 0, sizeof(*ctx));
}

/*
 * Algorithm 1: Wave Propagation
 * ----------------------------------------------------------------------------
 * Se ejecuta en un ciclo hasta un punto fijo (changed == false), 
 * es decir, hasta que ya no se encuentren nuevas relaciones de punteros.
 *   1) Colapsa componentes fuertemente conectados (SCC) el grafo se vuelve aciclico.
 *   2) Propaga puntos-a en orden topologico (usando T).
 *   3) Evalua constraints complejas y agrega nuevas aristas.
 */
void wave_Propagation(Graph **G) {
    if (!G || !*G) { printf("Grafo vacio"); return; }
    bool changed;
    do {
        WPContext ctx;
        wpctx_init(&ctx, *G);
        changed = false;
        collapseSCC(G, &ctx);
        perform_Wave_Propagation(&ctx);
        changed = add_new_edges(G);
        wpctx_destroy(&ctx);
    } while (changed);
}

/*
 * mergeNodes: combina el conjunto de referencias
 * (Pcur/Pold) de w dentro de v.
 * - Utiliza set_union y reemplazamos v->references por la union.
 */
static void mergeNodes(Node *target, Node *source) {
    // t = t u s
    set_union_inplace(&Pcur(target), Pcur(source));
    set_union_inplace(&Pold(target), Pold(source));
}

/* Agrega las edges salientes de source en target */
static void out_edges_in_target(Node *target, Node *source) {
    for (Set *e = source->edges; e; e = e->next) {
        node_addEdge(target, e->node);
    }
}

/*
 * unify(g, v, w)
 * ----------------------------------------------------------------------------
 * Colapsa el nodo source dentro del nodo target.
 *
 * Que actualiza:
 *   - Aristas entrantes source->w  se reemplazan por target->w.
 *   - Referencias (points-to) hacia source se redirigen a target.
 *   - Se elimina el posible autociclo target->target si aparece.
 *   - Se fusionan Pcur/Pold: target := (target ∪ source) (mergeNodes).
 *   - Se elimina nodo source del grafo.
 */
static void unify(Graph **G, Node *target, Node *source) {
    if (target == source) return;

    unify_node_to_target(*G, target, source);

    // 2) Remapear cache
    constraints_remap_cache(listComplex1, source, target);
    constraints_remap_cache(listComplex2, source, target);
    // 3) las edges salientes de sources, agregarlas a target
    out_edges_in_target(target,source);
    //4) Eliminar posible autociclo generado
    node_removeEdge(target, target);
    // 5) Fusionar info (Pcur/Pold, etc.)
    mergeNodes(target, source);
    node_alias_merge(target, source);

    //6) eliminar el nodo del grafo y destruir ese nodo
    removeNode(G, source);
    node_destroy(source);
}

/*
 * Algoritmo 2: Colapso de Componentes Fuertemente Conectados (SCC) 
 *   Detecta componentes fuertemente conectados y los colapsa a un representante,
 *   dejando G como aciclico. Ademas, contruye T con los representantes en
 *   orden topologico (T se usa luego para propagar).
 * ----------------------------------------------------------------------------
 *   - Inicializa D,R,S,T,C; reinicia I.
 *   - Recorre todos los nodos no visitados con visitNode.
 *   - Unifica (unify) todo v cuyo R(v) != v con su representante R(v).
 */
void collapseSCC(Graph **G, WPContext *ctx) {
    // Primera fase: Visitar nodos no visitados
    for (Graph *curGraph = *G; curGraph; curGraph = curGraph->next) {
        if (getDValue(ctx->D, curGraph->node) == UNVISITED) {
            visitNode(curGraph->node, ctx);
        }
    }
    // Segunda fase: Collapse Strongly Connected Components (SCCs)
    // Para cada v con R(v) != v, unify(v, R(v)) -> mover aristas/referencias a representative.
    Graph *next = NULL;
    for (Graph *curGraph = *G; curGraph; curGraph = next) {
        Node *v = curGraph->node;
        Node *r = getRValue(ctx->R, v);
        next = curGraph->next;
        if (r != v) {
            unify(G, r, v);
        }
    }
}

/*
 * Algorithm 3: visitNode
 * ----------------------------------------------------------------------------
 * Realiza una busqueda en profundidad (DFS) para encontrar Componentes 
 * Fuertemente Conectados (ciclos) y construir un orden topologico.
 * 
 *   - D(v) marca el orden DFS (cuando visitamos el nodo).
 *   - R(v) Representante del nodo (apunta a la raiz del ciclo).
 *   - S Pila de nodos ya explorados.
 *   - C Conjunto de nodos que ya fueron procesados y cerrados.
 *   - T Pila final donde guardamos los representantes en orden topologico.
 *
 *   1. Exploramos los sucesores de v. Si encontramos una conexion hacia atras
 *   (un nodo con un D menor), actualizamos nuestro representante R(v).
 *   2. Si al terminar de explorar resulta que R(v) = v, significa que v es la raiz del ciclo.
 *   Sacamos de la pila S todos los nodos de ese ciclo, los asignamos a v como su representante
 *   y guardamos v en T.
 */
void visitNode(Node* v, WPContext *ctx) {
    ctx->I++;
    setDValue(ctx->D, v, ctx->I);    // D(v) <- I
    setRValue(ctx->R, v, v);     // R(v) <- v inicialmente
    // Recorremos todos los sucesores (v,w) ∈ E
    Set *edge = v->edges;
    while (edge != NULL) {
        Node *w = edge->node;
        // Si w no visitado, recursivamente lo visitamos
        if (getDValue(ctx->D, w) == UNVISITED) 
            visitNode(w, ctx);

        // Solo consideramos R(w) si w no esta ya en C 
        if (!set_existElem(ctx->C,w)) {
            //R(v) ← (D(R(v)) < D(R(w))) ? R(v) : R(w)
            Node *RvalueInV = getRValue(ctx->R,v);
            Node *RvalueInW = getRValue(ctx->R,w);
            if (getDValue(ctx->D, RvalueInV) > getDValue(ctx->D, RvalueInW)) {
                setRValue(ctx->R, v, RvalueInW);
            }
        }
        edge = edge->next;
    }
    // Si R(v) == v entonces v es representante de una SCC
    if (getRValue(ctx->R,v) == v) {
        set_addElem(&ctx->C,v);  // marcar v en C

        // Extraer de S todos los nodos con D(w) > D(v) y unificarlos a v
        while (!stack_isEmpty(ctx->S)) {
            //w es el nodo en el top de la pila S
            Node *w = stack_top(ctx->S);
            if (getDValue(ctx->D, w) <= getDValue(ctx->D, v)) {
                break;
            } else {
              stack_pop(ctx->S);
              set_addElem(&ctx->C,w);    // añadir w a C
              setRValue(ctx->R, w, v);   // R(w) <- v (representante)
            }
        }
        stack_push(ctx->T, v);
    }else {
        stack_push(ctx->S,v);
    }
}


/*
 * Algorithm 4: perform_Wave_Propagation
 * Transmite la informacion a la que apunta cada nodo hacia sus sucesores, 
 * procesandolos en orden topologico (utilizando la pila T).
 * 
 * - Propaga las diferencias Pdif = Pcur(v) - Pold(v) a los sucesores de v, recorriendo T.
 *
 * - Mantiene el invariante: Pold(v) ⊆ Pcur(v).
*/
void perform_Wave_Propagation(WPContext *ctx) {

    while(!stack_isEmpty(ctx->T)) {
        Node *v = stack_top(ctx->T);
        stack_pop(ctx->T);
        //Pdif ← Pcur(v) − Pold(v)
        Set *pdif = set_difference(Pcur(v), Pold(v));
        //Pold(v) ← Pcur(v)
        set_destroy(&Pold(v));
        Pold(v) = set_clone(Pcur(v));
        //por cada w tal que (v,w) ∈ E, Pcur(w) ← Pcur(w) ∪ Pdif
        for (Set *edge = v->edges; edge; edge = edge->next) {
            Node* w = edge->node;
            set_union_inplace(&Pcur(w), pdif);
        }
        set_destroy(&pdif);
    }
}


/*
 * Algorithm 5: add_new_edges
 * ----------------------------------------------------------------------------
 * Evalua las restricciones complejas (punteros desreferenciados).
 * Si un puntero ahora apunta a nuevos lugares, se generan nuevas aristas en el grafo.
 * 
 * Devuelve true si se agrego alguna arista nueva, 
 * lo que forzara una nueva iteracion del algoritmo WP.
 * 
 *   - Complex 1 (LOAD/Lectura):  l ⊇ *r (l = *r)
 *      Si r apunta a v, entonces l debe tener todo lo que contenga v.
 *      Es decir, por cada nuevo nodo v en el conjunto points-to de r, agregamos la arista v -> l.
 *
 *   - Complex 2 (STORE/Escritura):  *l ⊇ r (*l = r)
 *      Si l apunta a v, entonces v debe tener todom lo que tenga r.
 *      Es decir, por cada nuevo nodo v en el conjunto points-to de l, agregamos la arista r -> v.
 * 
 * Utiliza la variable Pcache para no evaluar aristas que ya procesamos en iteraciones anteriores.
 */

bool add_new_edges(Graph **G) {
    bool changed = false;

    // Complex 1:  l ⊇ *r
    for (ListConstraint *curCons = listComplex1; curCons; curCons = curCons->next) {
        char *lname             = constraint_getL(curCons);
        char *rname             = constraint_getR(curCons);

        Graph *gl = findNode(*G, lname);
        Graph *gr = findNode(*G, rname);
        if (!gl || !gr) continue;
        Node *l = gl->node;
        Node *r = gr->node;

        //Pnew ← Pcur(r) − Pcache(c)
        Set *pNew           = set_difference(Pcur(r), curCons->pcache);
        //Pcache(c) ← Pcache(c) ∪ Pnew
        set_union_inplace(&curCons->pcache, pNew);

        // for v ∈ Pnew do …
        for (Set* curSet = pNew; curSet ; curSet = curSet->next) {
            /*(v,l) /∈ E */
            Node *v = curSet->node;
            if(v != l && !node_existEdge(v, l)) {
                node_addEdge(v,l);
                changed = true;
                set_union_inplace(&Pcur(l), Pold(v));
            }
        }
        set_destroy(&pNew);
    }
    
    //Complex 2
    for (ListConstraint *curCons = listComplex2; curCons; curCons = curCons->next) {
        char *lname = constraint_getL(curCons);
        char *rname = constraint_getR(curCons);

        Graph *gl = findNode(*G, lname);
        Graph *gr = findNode(*G, rname);
        if (!gl || !gr) continue;
        Node *l = gl->node;
        Node *r = gr->node;

        Set *pNew = set_difference(Pcur(l), curCons->pcache);
        set_union_inplace(&curCons->pcache, pNew);

        for (Set *curSet = pNew; curSet; curSet = curSet->next) {
            /*(r, v) /∈ E  */
            Node *v = curSet->node;
            if(r != v && !node_existEdge(r,v)) {
                node_addEdge(r,v);
                changed = true;
                set_union_inplace(&Pcur(v), Pold(r));
            }
        }
        set_destroy(&pNew);
    }
    return changed;
}