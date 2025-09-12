#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "../include/Graph.h"
#include "../include/Stack.h"
#include "../include/Map.h"
#include "../include/Set.h"
#include "../include/Stack.h"
#include "ListConstraint.h"

/* MACROS */
#define Pcur(n) ((n)->references)
#define Pold(n) ((n)->pold)

extern ListConstraint *listComplex1;                    // l ⊇ *r
extern ListConstraint *listComplex2;                    // *l ⊇ r

void wave_Propagation(Graph **G);                       //Algorithm 1
void collapseSCC(Graph **G);                            //Algorithm 2
void mergeNodes(Node *v, Node *w);                      
void visitNode(Node* v, int *I);                        //Algorithm 3
void unify(Graph **G, Node *target, Node *source);      //Unifica los nodos para que se vuelvan 1 solo.
void perform_Wave_Propagation();                        //Algorithm 4
void propagationTo(Node *w, Set *pdif);                 //Propaga el conjunto de apuntados a un nodo en particular.
bool add_new_edges();                                   //Algorithm 5
void remap_constraints_after_unify(Node *oldw, Node *rep);

//Helper
char *str_concat(const char *a, const char *b);
#endif
