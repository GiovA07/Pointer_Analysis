#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <stdlib.h>
#include <stdbool.h>
#include "../include/Graph.h"
#include "../include/ListConstraint.h"
#include "../include/Map.h"
#include "../include/Set.h"
#include "../include/Stack.h"

typedef struct {
    // Mapas
    DMap  *D;
    RMap  *R;
    // Estructuras SCC
    Set   *C;
    Stack *S;
    Stack *T;
    // Contador DFS
    int I;
} WPContext;


void wave_Propagation(Graph **G);                   //Algorithm 1
void collapseSCC(Graph **G, WPContext *ctx);        //Algorithm 2
void visitNode(Node* v, WPContext *ctx);            //Algorithm 3
void perform_Wave_Propagation(WPContext *ctx);      //Algorithm 4
bool add_new_edges(Graph **G);                      //Algorithm 5

#endif
