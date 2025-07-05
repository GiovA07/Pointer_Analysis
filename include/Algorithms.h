#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../include/Graph.h"
#include "../include/Stack.h"
#include "../include/Map.h"
#include "../include/Set.h"
#include "../include/Stack.h"

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

void wave_Propagation(Graph *G); //Algoritmo 1
void collapseSCC(Graph *G); //Algoritmo 2
void visitNode(Node* v, int *I); //Algoritmo 3
void unify(Graph* g, Node* v, Node* w);
void perform_Wave_Propagation(); //Algoritmo 4
#endif
