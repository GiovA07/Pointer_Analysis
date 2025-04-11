#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../include/Graph.h"
#include "../include/Stack.h"
#include "../include/Map.h"
#include "../include/Set.h"
#include "../include/Stack.h"

// Mapas para D y R
DMap *D;  // Orden de visita de cada nodo
RMap *R;  // Representante de SCC

//SET C
Set *C;
//STACK
Stack *S;
Stack *T;

int I = 0;  // Contador global

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
            //unify(currentGraph->node, getRValue(currentGraph->node))                                                       //TODAVIA NO LO IMPLEMENTE
        }
        currentGraph = currentGraph->next;
    }
}


//Algorithm 3 Visit Node
void visitNode(Node* v, int *I) {
    (*I)++;
    setDValue(D, v, I);
    setRValue(R, v, v);

    //Para todo w tal que (node,w) ∈ edges
    Node* edges[MAX_NODES] = v->edges;

    for (int i = 0; i < MAX_NODES && edges[i] != NULL; i++) {
        Node *w = edges[i];
        if (getDValue(D, w) == UNVISITED) visitNode(w, I);

        if (!existsInSet(C,w)) {
            //R(v) ← (D(R(v)) < D(R(w))) ? R(v) : R(w)
            Node *RvalueInV = getRValue(R,v);
            Node *RvalueInW = getRValue(R,w);
            if (getDValue(D, RvalueInV) > getDValue(D, RvalueInW)) {
                setRValue(R, v, RvalueInW);
            }
            
        }
    }
    
    if (getRValue(R,v) == v) {
        addToCSet(C,v);

        while (!isEmpty(S)) {
            //let w be the node on the top of S
            Node *w = top(S);
            if (getDValue(D, w) <= getDValue(D, v)) {
                break;
            } else {
              pop(S);
              addToCSet(C,w);
              setRValue(R, w, v);  
            }
        }
        push(T, v);
    }else {
        push(S,v);
    }
}


//Algorithm 4
