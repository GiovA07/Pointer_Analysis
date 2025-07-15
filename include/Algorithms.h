#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../include/Graph.h"
#include "../include/Stack.h"
#include "../include/Map.h"
#include "../include/Set.h"
#include "../include/Stack.h"
#include "ListConstraint.h"


#ifndef ALGORITHMS_H
#define ALGORITHMS_H


extern ListConstraint* listComplex1;
extern ListConstraint* listComplex2;

void wave_Propagation(Graph *G);        //Algorithm 1
void collapseSCC(Graph *G);             //Algorithm 2
void mergeNodes(Node *v, Node *w);
void visitNode(Node* v, int *I);        //Algorithm 3
void unify(Graph* g, Node* v, Node* w);
void perform_Wave_Propagation();        //Algorithm 4
void propagationTo(Node *w, Set *pdif);
void add_new_edges();                   //Algorithm 5
#endif
