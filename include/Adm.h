#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Graph.h"
#include "./Node.h"

#ifndef ADM_H
#define ADM_H

// Funciones para los constraints
void constraitBaseAlg(Graph **g, char *nameNode1, char *nameNode2);
void constraintSimpleAlg(Graph **g, char *nameNode1, char *nameNode2);

// int constraintComplex1(Graph *g, Node *node);
// void constraintComplex2 (Graph *g, Node *from, Node *to);

#endif

