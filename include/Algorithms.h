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

void wave_Propagation(Graph *G);
void collapseSCC(Graph *G);
void visitNode(Node* v, int *I);
void unify(Graph* g, Node* v, Node* w);

#endif
