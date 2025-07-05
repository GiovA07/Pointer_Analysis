
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
    Graph *currentGraph = G;
    while (currentGraph != NULL) {
        if (getDValue(D, currentGraph->node) == UNVISITED) {
            visitNode(currentGraph->node, &I);
        }
        currentGraph = currentGraph->next;
    }

    currentGraph = G;
    // Segunda fase: Colapsar SCCs
    while (currentGraph != NULL) {
        if (getRValue(R, currentGraph->node) != currentGraph->node) {       //Ver si los nodos puedo compararlos asi (creeria que si ya que contienen la misma direccion de memoria porque no se crean nuevos nodos)
            unify(G,currentGraph->node, getRValue(R,currentGraph->node));                                                       //TODAVIA NO LO IMPLEMENTE
        }
        currentGraph = currentGraph->next;
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

    //remove el mismo
    removeEdgeInNode(v, v);

    //junta los apuntados
    Set *references = w->references;
    while (references!=NULL) {
        set_addElem(&v->references, references->node);
        references = references->next;
    }
    
    removeNode(&g, w);
    
}

//Algorithm 4
void perform_Wave_Propagation() {

    while(!isEmpty(T)) {
        Node *v = top(T);
        pop(T);
        Set *pdif = set_difference(v->references, v->pold);
        Set *new_pold = set_clone(v->references);
        set_destroy(v->pold);
        v->pold = new_pold;

        Set* currentEdge = v->edges;
        while (currentEdge != NULL) {
            Node* w = currentEdge->node;
            Set *ref = w->references;
            w->references = set_union(w->references, pdif);
            currentEdge = set_nextElem(currentEdge);
            set_destroy(ref);
        }

        set_destroy(pdif);
    }
}

