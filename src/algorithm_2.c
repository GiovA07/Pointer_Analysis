#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "Graph.h"
#include "Stack.h"
#include "Map.h"


#define UNVISITED -1  // Representa ⊥

// Mapas para D y R
DMap *D;  // Orden de visita de cada nodo
RMap *R;  // Representante de SCC

int I = 0;  // Contador global

// Algorithm 2: Collapse SCCs
void collapseSCC(Graph *G) {

    D = initDMap(G);
    R = initRMap(G);
    Stack *S = createStack();

    // Primera fase: Visitar nodos no visitados
    Graph *currentGraph = G;
    while (currentGraph != NULL) {
        if (getDValue(D, currentGraph->node) == UNVISITED) {
            //ALGORITMO 3 QUE NO IMPLEMENTE TODAVIA
        }
    }

    currentGraph = G;
    // Segunda fase: Colapsar SCCs
    while (currentGraph != NULL) {
        if (getRValue(R, currentGraph->node) != currentGraph->node) {       //Ver si los nodos puedo compararlos asi (creeria que si ya que contienen la misma direccion de memoria porque no se crean nuevos nodos)
            //unify(currentGraph->node, getRValue(currentGraph->node))                                                       //TODAVIA NO LO IMPLEMENTE
        }
    }
}
