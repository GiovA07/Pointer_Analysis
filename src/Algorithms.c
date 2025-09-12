
#include "../include/Algorithms.h"

// Mapas para D y R
DMap *D;
RMap *R;
// SET C y STACKS S, T usadas en (Alg.2/Alg.3)
Set *C;
Stack *S;
Stack *T;
// contador global para numerar nodos (D)
int I;

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
        changed = false;
        collapseSCC(G);
        perform_Wave_Propagation();
        changed = add_new_edges();
    } while (changed == true);
    printf("Termino \n");
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
void collapseSCC(Graph **G) {
    D = initDMap(*G);        // todos D(v) = UNVISITED (⊥)
    R = initRMap(*G);        // R(v) = v inicialmente
    S = createStack();
    T = createStack();
    C = createSet();
    I = 0;

    // Primera fase: Visitar nodos no visitados
    for (Graph *curGraph = *G; curGraph; curGraph = curGraph->next) {
        if (getDValue(D, curGraph->node) == UNVISITED) {
            visitNode(curGraph->node, &I);
        }
    }
    // Segunda fase: Collapse Strongly Connected Components (SCCs)
    // Para cada v con R(v) != v, unify(v, R(v)) -> mover aristas/referencias a representative.
    Graph *next = NULL;
    for (Graph *curGraph = *G; curGraph; curGraph = next) {
        Node *v = curGraph->node;
        Node *r = getRValue(R, v);
        next = curGraph->next;  
        if (r != v) {
            unify(G, r, v);
        }
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
void unify(Graph **G, Node *target, Node *source) {
    if (target == source) return;
    char *merged_name = str_concat(target->name, source->name);

    for(Graph *curGraph = *G; curGraph; curGraph = curGraph->next) {
        Node *curNode = curGraph->node;
        // Si currentNode tiene una arista hacia w, reemplazarla por una arista hacia v.
        if(curNode && set_existElem(curNode->edges, source)) {
            addEdgeInNode(curNode, target);
            removeEdgeInNode(curNode, source);
        }
        // redirigir referencias a w por rep
        if(curNode && set_existElem(Pcur(curNode), source)) {
            removeReference(curNode, source);
            addReference(curNode, target);
        }
    }

    // 2) Remapear constraints complejas (L/R y cache si hace falta)
    remap_constraints_after_unify(source,target);
    //3) Eliminar posible autociclo generado
    removeEdgeInNode(target, target);
    // 4) Fusionar info (Pcur/Pold, etc.)
    mergeNodes(target, source);

    // — renombrar el representante —
    free(target->name);
    target->name = merged_name;

    // eliminar w del grafo
    removeNode(G, source);

}

void remap_constraints_after_unify(Node *oldw, Node *rep) {
    // Complex 1: l ⊇ *r
    constraints_remap_nodes(listComplex1, oldw, rep);
    // Complex 2: *l ⊇ r
    constraints_remap_nodes(listComplex2, oldw, rep);
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
void visitNode(Node* v, int *I) {
    (*I)++;
    setDValue(D, v, *I);    // D(v) <- I
    setRValue(R, v, v);     // R(v) <- v inicialmente
    // Recorremos todos los sucesores (v,w) ∈ E
    Set *edge = v->edges;
    while (edge != NULL) {
        Node *w = edge->node;
        // Si w no visitado, recursivamente lo visitamos
        if (getDValue(D, w) == UNVISITED) 
            visitNode(w, I);

        // Solo consideramos R(w) si w no esta ya en C 
        if (!set_existElem(C,w)) {
            //R(v) ← (D(R(v)) < D(R(w))) ? R(v) : R(w)
            Node *RvalueInV = getRValue(R,v);
            Node *RvalueInW = getRValue(R,w);
            if (getDValue(D, RvalueInV) > getDValue(D, RvalueInW)) {
                setRValue(R, v, RvalueInW);
            }
        }
        edge = edge->next;
    }
    // Si R(v) == v entonces v es representante de una SCC
    if (getRValue(R,v) == v) {
        set_addElem(&C,v);  // marcar v en C

        // Extraer de S todos los nodos con D(w) > D(v) y unificarlos a v
        while (!isEmpty(S)) {
            //let w be the node on the top of S
            Node *w = top(S);
            if (getDValue(D, w) <= getDValue(D, v)) {
                break;
            } else {
              pop(S);
              set_addElem(&C,w);    // añadir w a C
              setRValue(R, w, v);   // R(w) <- v (representante)
            }
        }
        push(T, v);
    }else {
        push(S,v);
    }
}

/*
 * mergeNodes: combina el conjunto de referencias
 * (Pcur/Pold u otros sets relacionados) de w dentro de v.
 * - usamos set_union y reemplazamos v->references por la unión.
 */
void mergeNodes(Node *target, Node *source) {
    Set *mergedRefs = set_union(Pcur(target), Pcur(source));
    Set *mergedOld = set_union(Pold(target), Pold(source));
    set_destroy(Pcur(target));
    set_destroy(Pcur(source));
    set_destroy(Pold(target));
    set_destroy(Pold(source));
    Pcur(target) = mergedRefs;
    Pold(target) = mergedOld;
}

// Propaga el conjunto pdif desde v a w
void propagationTo(Node *w, Set *pdif) {
    Set *oldRefs = Pcur(w);
    Pcur(w) = set_union(oldRefs, pdif);
    // REVIEW:	ver si es correcto clonar el pold aca
    Pold(w) = set_clone(Pcur(w));
    set_destroy(oldRefs);
}

/*
 * Algorithm 4: perform_Wave_Propagation
 * - Propaga las diferencias Pdif = Pcur(v) - Pold(v) a los sucesores de v,
 *   recorriendo T.
 * - Mantiene el invariante Pold(v) ⊆ Pcur(v).
*/
void perform_Wave_Propagation() {

    while(!isEmpty(T)) {
        Node *v = top(T);
        pop(T);
        //Pdif ← Pcur(v) − Pold(v)
        Set *pdif = set_difference(Pcur(v), Pold(v));
        //Pold(v) ← Pcur(v)
        set_destroy(Pold(v));
        Pold(v) = set_clone(Pcur(v));
        //por cada w tal que (v,w) ∈ E, Pcur(w) ← Pcur(w) ∪ Pdif
        for (Set *edge = v->edges; edge; edge = edge->next) {
            Node* w = edge->node;
            propagationTo(w, pdif);
        }

        set_destroy(pdif);
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

bool add_new_edges() {
    bool add_edges = 0;
    //Complex 1
    ListConstraint *currentConstraint = listComplex1;
    while (currentConstraint) {
        Node *l             = constraint_getL(currentConstraint);
        Node *r             = constraint_getR(currentConstraint);
        Set *pCache         = constraint_getCache(currentConstraint);

        //Pnew ← Pcur(r) − Pcache(c)
        Set *pNew           = set_difference(Pcur(r), pCache);
        //Pcache(c) ← Pcache(c) ∪ Pnew
        Set *unionCacheNew  = set_union(pCache, pNew);
        constraint_setCache(currentConstraint,unionCacheNew);
        
        // for v ∈ Pnew do …
        Set *pNewIter = pNew; 
        while (pNewIter) {
            /*(v,l) /∈ E */
            Node *v = pNewIter->node;
            if(v != l && !existEdgeInNode(v, l)) {
                addEdgeInNode(v,l);
                add_edges = true;
                Set *newPCurL = set_union(Pcur(l), Pold(v));
                node_setReferences(l, newPCurL);
            }
            pNewIter = set_nextElem(pNewIter);
        }
        set_destroy(pNew);
        //set_destroy(pCache);
        currentConstraint = constraint_getNext(currentConstraint);
    }
    
    //Complex 2
    currentConstraint = listComplex2;
    while (currentConstraint) {
        Node *l = constraint_getL(currentConstraint);
        Node *r = constraint_getR(currentConstraint);

        Set *pCache = constraint_getCache(currentConstraint);
        Set *pNew = set_difference(Pcur(l), pCache);
        
        Set *unionCacheNew = set_union(pCache, pNew);
        constraint_setCache(currentConstraint,unionCacheNew);

        Set *pNewIter = pNew; 
        while (pNewIter) {
            /*(r, v) /∈ E  */
            Node *v = pNewIter->node;
            if(r != v && !existEdgeInNode(r,v)) {
                addEdgeInNode(r,v);
                add_edges = true;
                Set *newPCurV = set_union(Pcur(v), Pold(r));
                node_setReferences(v, newPCurV);
            }
            pNewIter = set_nextElem(pNewIter);
        }
        set_destroy(pNew);
        //set_destroy(pCache);  ///VER QUE HACER CON ESTO (porque causa error)
        currentConstraint = constraint_getNext(currentConstraint);
    }
    return add_edges;
}


char *str_concat(const char *a, const char *b) {
    size_t la = strlen(a);
    size_t lb = strlen(b);
    char *out = (char *)malloc(la + lb + 1);  // +1 para el '\0'
    if (!out) return NULL;
    memcpy(out, a, la);
    memcpy(out + la, b, lb);
    out[la + lb] = '\0';
    return out;
}