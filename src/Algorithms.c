
#include "../include/Algorithms.h"


// Mapas para D y R
DMap *D;  // Orden de visita de cada nodo
RMap *R;  // Representante de SCC

// SET C y STACKS S, T usadas en (Alg.2/Alg.3)
Set *C;
Stack *S;
Stack *T;

int I = 0;  // contador global para numerar nodos (D)


/*
 * Algorithm 1: Wave Propagation
 * - Repite: colapsar SCCs, propagar (onda) en orden tropologico y añadir nuevas aristas.
 */
void wave_Propagation(Graph *G) {
    collapseSCC(G);
    perform_Wave_Propagation();
    add_new_edges();
    printf("termino\n");
}

// Algorithm 2: Collapse SCCs
void collapseSCC(Graph *G) {

    D = initDMap(G);        // todos D(v) = UNVISITED (⊥)
    R = initRMap(G);        // R(v) = v inicialmente
    S = createStack();
    T = createStack();
    C = createSet();

    // Primera fase: Visitar nodos no visitados
    for (Graph *curGraph = G; curGraph; curGraph = curGraph->next) {
        if (getDValue(D, curGraph->node) == UNVISITED) {
            visitNode(curGraph->node, &I);
        }
    }

    // Segunda fase: Colapsar SCCs
    // Para cada v con R(v) != v, unify(v, R(v)) -> mover aristas/referencias a representative.
    for (Graph *curGraph = G; curGraph; curGraph = curGraph->next) {
        Node *v = curGraph->node;
        Node *r = getRValue(R, v);
        if (r != v)
            unify(G, v, r);
    }
}



/*
 * Algorithm 3: visitNode
 * Entradas:
 *   v: nodo actual
 *   I: puntero al contador global (se incrementa para asignar D)
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
                // encontramos uno que no pertenece a esta SCC
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

/*La unica forma en la que pienso esto, es viendo tanto REFERENCES y EDGES*/
/*
 * unify: Mueve aristas y referencias de w hacia v y elimina w del grafo.
 * - Es la operacion que realiza el "colapso" del nodo w dentro de v.
 * - Debe actualizar:
 *    - Aristas entrantes: si x->w entonces ahora x->v (remover x->w).
 *    - Referencias (Pcur/Pold etc): trasladarlas a v.
 *    - Eliminar autociclos (v->v) si aparecen.
 *    - mergeNodes(v,w) para fusionar estructuras (refs).
 *    - removeNode(&g, w) para eliminar w.
 *
 */

void unify(Graph* g, Node* v, Node* w){
    Graph *currentGraph = g;
    while (currentGraph) {
        Node *currentNode = currentGraph->node;
        // Si currentNode tiene una arista hacia w, reemplazarla por una arista hacia v.
        if(currentNode && set_existElem(currentNode->edges, w)) {
            addEdgeInNode(currentNode, v);
            removeEdgeInNode(currentNode, w);
        }
        // Si currentNode tiene una referencia (Pcur/Pold) hacia w, reemplazarla por v.
        if(currentNode && set_existElem(currentNode->references, w)) {
            removeReference(currentNode, w);
            addReference(currentNode, v);
        }

        currentGraph = currentGraph->next;
    }

    // Eliminar posible autociclo creado (si v apuntaba a w y ahora apunta a si mismo)
    removeEdgeInNode(v, v);

    // Fusionar las referencias de w en v
    mergeNodes(v, w);
    // Finalmente eliminar el nodo w del grafo
    removeNode(&g, w);
    
}


/*
 * mergeNodes: combina el conjunto de referencias
 * (Pcur/Pold u otros sets relacionados) de w dentro de v.
 * - usamos set_union y reemplazamos v->references por la unión.
 */
void mergeNodes(Node *v, Node *w) {
    // set_union devuelve el nuevo set de referencias
    Set *mergedRefs = set_union(v->references, w->references);
    set_destroy(v->references);
    v->references = mergedRefs;
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
        Set *pdif = set_difference(v->references, v->pold);
        //Pold(v) ← Pcur(v)
        set_destroy(v->pold);
        v->pold = set_clone(v->references);

        //por cada w tal que (v,w) ∈ E, Pcur(w) ← Pcur(w) ∪ Pdif
        for (Set *edge = v->edges; edge; edge = edge->next) {
            Node* w = edge->node;
            propagationTo(w, pdif);
        }

        set_destroy(pdif);
    }
}

// Propaga el conjunto pdif desde v a w
void propagationTo(Node *w, Set *pdif) {
    Set *oldRefs = w->references;
    w->references = set_union(oldRefs, pdif);
    set_destroy(oldRefs);
}

/*Algorith 5 
* - Recorre las constraints de tipo Complex1 (l ⊇ *r) y Complex2 (*l ⊇ r)
* - Para cada constraint mantiene un Pcache(c) (cache de nodos ya procesados).
* - Calcula Pnew = Pcur(r) - Pcache(c) y para cada v ∈ Pnew:
*     - si (v,l) ∉ E entonces añade la arista y actualiza Pcur(l) con Pold(v)
* - Análogo para Complex2.
*/
void add_new_edges() {
    //Complex 1
    ListConstraint *currentConstraint = listComplex1;
    while (currentConstraint) {
        Node *l             = constraint_getL(currentConstraint);
        Node *r             = constraint_getR(currentConstraint);

        Set *pCache         = constraint_getCache(currentConstraint);
        //Pnew ← Pcur(r) − Pcache(c)
        Set *pNew           = set_difference(r->references, pCache);
        //Pcache(c) ← Pcache(c) ∪ Pnew
        Set *unionCacheNew  = set_union(pCache, pNew);
        constraint_setCache(currentConstraint,unionCacheNew);
        
        // for v ∈ Pnew do …
        Set *pNewIter = pNew; 
        while (pNewIter) {
            /*(v,l) /∈ E */
            Node *v = pNewIter->node;

            if(!existEdgeInNode(v, l)) {
                addEdgeInNode(v,l);
                Set *newPCurL = set_union(l->references, v->pold);
                node_setReferences(l, newPCurL);
            }
            pNewIter = set_nextElem(pNewIter);
        }
        set_destroy(pNew);
        set_destroy(pCache);
        currentConstraint = constraint_getNext(currentConstraint);
    }
    
    //Complex 2
    currentConstraint = listComplex2;
    while (currentConstraint) {
        Node *l = constraint_getL(currentConstraint);
        Node *r = constraint_getR(currentConstraint);

        Set *pCache = constraint_getCache(currentConstraint);
        Set *pNew = set_difference(l->references, pCache);
        
        Set *unionCacheNew = set_union(pCache, pNew);
        constraint_setCache(currentConstraint,unionCacheNew);

        Set *pNewIter = pNew; 
        while (pNewIter) {
            /*(r, v) /∈ E  */
            Node *v = pNewIter->node;

            if(!existEdgeInNode(r,v)) {
                addEdgeInNode(r,v);
                Set *newPCurV = set_union(v->references, r->pold);
                node_setReferences(v, newPCurV);
            }
            pNewIter = set_nextElem(pNewIter);
        }
        set_destroy(pNew);
        set_destroy(pCache);
        currentConstraint = constraint_getNext(currentConstraint);
    }
    
}
