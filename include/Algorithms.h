#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <stdlib.h>
#include <stdbool.h>
#include "../include/Graph.h"
#include "../include/ListConstraint.h"
#include "../include/Map.h"
#include "../include/Set.h"
#include "../include/Stack.h"

void wave_Propagation(Graph **G);                       //Algorithm 1
void collapseSCC(Graph **G);                            //Algorithm 2
void visitNode(Node* v, int *I);                        //Algorithm 3
void perform_Wave_Propagation();                        //Algorithm 4
bool add_new_edges(Graph **G);                          //Algorithm 5

#endif
