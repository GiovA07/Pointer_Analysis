
#include "../include/Algorithms.h"


// Mapas para D y R
DMap *D;  // Orden de visita de cada nodo
RMap *R;  // Representante de SCC

//SET C
Set *C;
//STACK
Stack *S;
Stack *T;

int I = 0;  // Contador global



//Algorithm 1: Wave Propagation

void wave_Propagation(Graph *G) {
    collapseSCC(G);
    perform_Wave_Propagation();
    add_new_edges();
    printf("termino\n");
}

// Algorithm 2: Collapse SCCs
void collapseSCC(Graph *G) {

    D = initDMap(G);
    R = initRMap(G);
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
    for (Graph *curGraph = G; curGraph; curGraph = curGraph->next) {
        Node *v = curGraph->node;
        Node *r = getRValue(R, v);
        if (r != v)
            unify(G, v, r);
    }
}


//Algorithm 3 Visit Node
void visitNode(Node* v, int *I) {
    (*I)++;
    setDValue(D, v, *I);
    setRValue(R, v, v);

    Set *edge = v->edges;
    while (edge != NULL) {
        Node *w = edge->node;
        if (getDValue(D, w) == UNVISITED) visitNode(w, I);

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
    
    if (getRValue(R,v) == v) {
        set_addElem(&C,v);

        while (!isEmpty(S)) {
            //let w be the node on the top of S
            Node *w = top(S);
            if (getDValue(D, w) <= getDValue(D, v)) {
                break;
            } else {
              pop(S);
              set_addElem(&C,w);
              setRValue(R, w, v);  
            }
        }
        push(T, v);
    }else {
        push(S,v);
    }
}

/*La unica forma en la que pienso esto, es viendo tanto REFERENCES y EDGES*/

void unify(Graph* g, Node* v, Node* w){
    Graph *currentGraph = g;
    while (currentGraph) {
        Node *currentNode = currentGraph->node;
        //Si algun nodo tiene una arista con w, pasa a la de v.
        if(currentNode && set_existElem(currentNode->edges, w)) {
            addEdgeInNode(currentNode, v);
            removeEdgeInNode(currentNode, w);
        }

        //Si algun nodo tiene una referencia con w, pasa tenerla con v.
        if(currentNode && set_existElem(currentNode->references, w)) {
            removeReference(currentNode, w);
            addReference(currentNode, v);
        }

        currentGraph = currentGraph->next;
    }

    //remove el autociclo si existe
    removeEdgeInNode(v, v);

    //junta los apuntados y elimina el nodo w del grafo
    mergeNodes(v, w);
    removeNode(&g, w);
    
}

void mergeNodes(Node *v, Node *w) {
    // set_union devuelve el nuevo set de referencias
    Set *mergedRefs = set_union(v->references, w->references);
    set_destroy(v->references);
    v->references = mergedRefs;
}

//Algorithm 4
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

//Algorith 5
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
