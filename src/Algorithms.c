
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
 *
 * Nota:
 *   El algoritmo completo se repite hasta que no haya cambios
 *   Por ahora hice solo 1 iteracion (para ver que funciona como el ejemplo del paper)
 */

void wave_Propagation(Graph *G) {
    int result;
    int num = 0;
    do {
        result = 0;
        printf("Colapse de Repeticion %d \n", num);
        collapseSCC(G);
        printf("Perform de Repeticion %d \n", num);
        perform_Wave_Propagation();
        printf("Añadir nuevos edges en Repeticion: %d \n", num);
        result = add_new_edges();
        num = num+1;
    } while (result == 1);
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
 *
 * Al terminar:
 *   - G queda aciclico.
 *   - T contiene (en su tope) nodos sin predecesores; al hacer pop iteramos
 *     en orden topológico de representantes.
 */
void collapseSCC(Graph *G) {

    D = initDMap(G);        // todos D(v) = UNVISITED (⊥)
    R = initRMap(G);        // R(v) = v inicialmente
    S = createStack();
    T = createStack();
    C = createSet();
    I = 0;

    printf("Inicializacion de las estructuras realizada \n");

    // Primera fase: Visitar nodos no visitados
    for (Graph *curGraph = G; curGraph; curGraph = curGraph->next) {
        if (getDValue(D, curGraph->node) == UNVISITED) {
            visitNode(curGraph->node, &I);
        }
    }

    printf("Visitar nodos no visitados realizada \n");

    // Segunda fase: Collapse Strongly Connected Components (SCCs)
    // Para cada v con R(v) != v, unify(v, R(v)) -> mover aristas/referencias a representative.
    for (Graph *curGraph = G; curGraph;) {
        Node *v = curGraph->node;
        Node *r = getRValue(R, v);
        Graph *next = curGraph->next;  
        if(v) 
            printf("v es: %s \n", v->name);
        if(r) 
            printf("r es: %s \n", r->name);
        if (r != v) {
            //  printf("El nodo v es: %s y el nodo r es: %s \n", v->name, r->name);
            //  printf("Pold de %s: \n", v->name);
            //  set_print(v->pold);
            // printf("Pold de %s: \n", r->name);
            //  set_print(r->pold);
            // printf("References de %s: \n", v->name);
            //  set_print(v->references);
            // printf("References de %s: \n", r->name);
            //  set_print(r->references);
            unify(G, r, v);
            printf("Termino el unify \n");
        }
        curGraph = next;
    }
    printf("SCCs realizada \n");
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

void unify(Graph* g, Node* v, Node* w){
    Graph *currentGraph = g;
    if (v == w) return;
    while (currentGraph) {
        Node *currentNode = currentGraph->node;
        // Si currentNode tiene una arista hacia w, reemplazarla por una arista hacia v.
        if(currentNode && set_existElem(currentNode->edges, w)) {
            addEdgeInNode(currentNode, v);
            removeEdgeInNode(currentNode, w);
        }
        // redirigir referencias a w por rep
        if(currentNode && set_existElem(currentNode->references, w)) {
            removeReference(currentNode, w);
            addReference(currentNode, v);
        }
        currentGraph = currentGraph->next;
    }

    //ahora modificar los CONTRAINT COMPLEX
    remap_constraints_after_unify(w,v);
    printf("El nodo %s ya no estara en las constraint, ahora sera: %s \n", w->name, v->name);

    // Eliminar posible autociclo creado (si v apuntaba a w y ahora apunta a si mismo)
    removeEdgeInNode(v, v);
    // fusionar info (Pcur/Pold, etc.)
    mergeNodes(v, w);
    // eliminar w del grafo
    removeNode(&g, w);

}


void remap_constraints_after_unify(Node *oldw, Node *rep) {
    // Complex 1: l ⊇ *r
    for (ListConstraint *c = listComplex1; c; c = constraint_getNext(c)) {
        // Remapear extremos L/R si apuntaban a w
        if (constraint_getL(c) == oldw) constraint_setL(c, rep);
        if (constraint_getR(c) == oldw) constraint_setR(c, rep);

        // Remapear el cache (reemplazar oldw por rep si estuviera)
        //no es necesario por ahora
    }

    // Complex 2: *l ⊇ r
    for (ListConstraint *c = listComplex2; c; c = constraint_getNext(c)) {
        if (constraint_getL(c) == oldw) constraint_setL(c, rep);
        if (constraint_getR(c) == oldw) constraint_setR(c, rep);

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
void mergeNodes(Node *v, Node *w) {
    // set_union devuelve el nuevo set de referencias
    // printf("Pold de %s: \n", v->name);
    // set_print(v->pold);
    // printf("Pold de %s: \n", w->name);
    // set_print(w->pold);
    Set *mergedRefs = set_union(v->references, w->references);
    Set *mergedOld = set_union(v->pold, w->pold);
    set_destroy(v->references);
    set_destroy(w->references);
    set_destroy(v->pold);
    set_destroy(w->pold);
    v->references = mergedRefs;
    v->pold = mergedOld;
}



// Propaga el conjunto pdif desde v a w
void propagationTo(Node *w, Set *pdif) {
    Set *oldRefs = w->references;
    w->references = set_union(oldRefs, pdif);
    w->pold = set_clone(w->references);
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
        Set *pdif = set_difference(v->references, v->pold);
        //Pold(v) ← Pcur(v)
        set_destroy(v->pold);
        v->pold = set_clone(v->references);

        //por cada w tal que (v,w) ∈ E, Pcur(w) ← Pcur(w) ∪ Pdif
        for (Set *edge = v->edges; edge; edge = edge->next) {
            Node* w = edge->node;
            // printf("Pcur de %s: \n", w->name);
            // set_print(w->references);
            // printf("Pdif de %s: \n", v->name);
            // set_print(pdif);
            propagationTo(w, pdif);
            // printf("La propagacion en %s fue: \n", w->name);
            // set_print(w->references);
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

int add_new_edges() {
    int add_edges = 0;
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
                                printf("Complex 1: \n");
        while (pNewIter) {
            /*(v,l) /∈ E */
            Node *v = pNewIter->node;
                                printf("Nodo v: %s \n", v->name);
                                printf("Nodo l: %s \n", l->name);
            if(v != l && !existEdgeInNode(v, l)) {
                addEdgeInNode(v,l);
                add_edges = 1;
                                printf("Agrego la nueva arista\n New PCur sera la union entre: \n Pcur :\n");
                                set_print(l->references);
                                printf("Pold :\n");
                                set_print(v->pold);
                Set *newPCurL = set_union(l->references, v->pold);
                                printf("Nuevo references para %s: \n", l->name);
                                set_print(newPCurL);
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
             printf("Complex 2: \n");
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
                                printf("Nodo v: %s \n", v->name);
                                printf("Nodo l: %s \n", l->name);
            if(r != v && !existEdgeInNode(r,v)) {
                                printf("Agrego la nueva arista\n New PCur sera la union entre: \n Pcur :\n");
                                set_print(v->references);
                                printf("Pold :\n");
                                set_print(r->pold);
                addEdgeInNode(r,v);
                add_edges = 1;
                Set *newPCurV = set_union(v->references, r->pold);
                node_setReferences(v, newPCurV);
                                printf("Nuevo references para %s: \n", l->name);
                                set_print(newPCurV);
            }
            pNewIter = set_nextElem(pNewIter);
        }
        set_destroy(pNew);
        //set_destroy(pCache);
        currentConstraint = constraint_getNext(currentConstraint);
        printf("Esto continua?");
    }
    return add_edges;
}
