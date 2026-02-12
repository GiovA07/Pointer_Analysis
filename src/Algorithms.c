
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
 *   1) Colapsa ciclos (SCC) => el grafo se vuelve aciclico.
 *   2) Propaga puntos-a en orden topológico (usando T), SOLO las diferencias.
 *   3) Evalua constraints complejas y agrega aristas nuevas si aparecen.
 */
void wave_Propagation(Graph **G) {
    if (!G || !*G) { printf("Grafo vacío"); return; }
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
 * (Pcur/Pold u otros sets relacionados) de w dentro de v.
 * - usamos set_union y reemplazamos v->references por la unión.
 */
static void mergeNodes(Node *target, Node *source) {
    // t = t u s
    set_union_inplace(&Pcur(target), Pcur(source));
    set_union_inplace(&Pold(target), Pold(source));
}

/* Las edges salientes de source en target */
static void out_edges_in_target(Node *target, Node *source) {
    for (Set *e = source->edges; e; e = e->next) {
        addEdgeInNode(target, e->node);
    }
}

/*
 * unify(g, v, w)
 * ----------------------------------------------------------------------------
 * Colapsa el nodo w dentro del representante v.
 *
 * Que actualiza:
 *   - Aristas entrantes x->w  se reemplazan por x->v.
 *   - Referencias (points-to) hacia w se redirigen a v.
 *   - Se elimina el posible autociclo v->v si aparece.
 *   - Se fusionan Pcur/Pold: v := (v ∪ w) (mergeNodes).
 *   - Se elimina w del grafo.
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
    removeEdgeInNode(target, target);
    // 5) Fusionar info (Pcur/Pold, etc.)
    mergeNodes(target, source);
    node_alias_merge(target, source);

    //6) eliminar el nodo del grafo y destruir ese nodo
    removeNode(G, source);
    node_destroy(source);
}

/*
 * Algorithm 2: Collapse SCCs
 *   Detecta componentes fuertemente conexos y colapsarlos a un representante,
 *   dejando G como aciclico. Además, contruye T con los representantes en
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
        next = curGraph->next;  //Este lo pongo aca porque puede borrarse el nodo al unificar y eso me arrina el algoritmo
        if (r != v) {
            unify(G, r, v);
        }
    }
}

/*
 * Algorithm 3: visitNode
 * ----------------------------------------------------------------------------
 * Entradas:
 *   v: nodo actual; *I: contador global de num. de visita.
 *
 * Idea:
 *   - D(v) marca el orden DFS.
 *   - R(v) rastrea el "minimo" representante visto desde v (posible raiz de SCC).
 *   - S apila nodos que podrian pertenecer a la SCC actual.
 *   - C marca nodos ya cerrados en alguna SCC.
 *
 * Casos:
 *   - Si R(v) == v:
 *       v es raiz de su SCC. “Cerramos” la SCC: sacamos de S los w con
 *       D(w) > D(v), los marcamos en C y seteamos R(w)=v.
 *       Luego pusheamos v en T (v es representante y aporta al orden topologico).
 *   - Si R(v) != v:
 *       v todavia depende de alguien “más antiguo”: se apila en S.
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
            //let w be the node on the top of S
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
 * - Propaga las diferencias Pdif = Pcur(v) - Pold(v) a los sucesores de v,
 *   recorriendo T.
 * - Mantiene el invariante Pold(v) ⊆ Pcur(v).
*/
void perform_Wave_Propagation(WPContext *ctx) {

    while(!stack_isEmpty(ctx->T)) {
        // v <-- pop node on top of T
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
 * Procesa constraints complejas:
 *   - Complex 1:  l ⊇ *r
 *       Pnew = Pcur(r) − Pcache(c)    // solo los nodos de r no vistos antes
 *       Para cada v en Pnew:
 *          si (v,l) no existe, agregar arista (v,l) y “sembrar” Pold(v) en l.
 *
 *   - Complex 2:  *l ⊇ r
 *       Pnew = Pcur(l) − Pcache(c)
 *       Para cada v en Pnew:
 *          si (r,v) no existe, agregar arista (r,v) y “sembrar” Pold(r) en v.
 *
 * Posible cambio que le voy hacer:
 *   Devolver un booleano para poder hacer el ciclo en el algorithm 1.
 */

bool add_new_edges(Graph **G) {
    bool changed = false;

    // Complex 1:  l ⊇ *r
    for (ListConstraint *curCons = listComplex1; curCons; curCons = curCons->next) {
        char *lname             = constraint_getL(curCons);
        char *rname             = constraint_getR(curCons);

        Graph *gl = findNodeResolved(*G, lname);
        Graph *gr = findNodeResolved(*G, rname);
        if (!gl || !gr) continue; // o crear nodos
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
            if(v != l && !existEdgeInNode(v, l)) {
                addEdgeInNode(v,l);
                changed = true;
                set_union_inplace(&Pcur(l), Pold(v));
            }
        }
        set_destroy(&pNew);
        //set_destroy(pCache);
    }
    
    //Complex 2
    for (ListConstraint *curCons = listComplex2; curCons; curCons = curCons->next) {
        char *lname = constraint_getL(curCons);
        char *rname = constraint_getR(curCons);

        Graph *gl = findNodeResolved(*G, lname);
        Graph *gr = findNodeResolved(*G, rname);
        if (!gl || !gr) continue;
        Node *l = gl->node;
        Node *r = gr->node;

        Set *pNew = set_difference(Pcur(l), curCons->pcache);
        set_union_inplace(&curCons->pcache, pNew);

        for (Set *curSet = pNew; curSet; curSet = curSet->next) {
            /*(r, v) /∈ E  */
            Node *v = curSet->node;
            if(r != v && !existEdgeInNode(r,v)) {
                addEdgeInNode(r,v);
                changed = true;
                set_union_inplace(&Pcur(v), Pold(r));
            }
        }
        set_destroy(&pNew);
        //set_destroy(pCache);  ///VER QUE HACER CON ESTO (porque causa error)
    }
    return changed;
}