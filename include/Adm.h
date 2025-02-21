#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Graph.h"
#include "./Node.h"
#include "./ListVar.h"

#ifndef ADM_H
#define ADM_H

// Funciones para la administracion
void constraintBase(Node* nodo, Node* ref);
void constraintSimple(Graph *g, Node* toEdge);
// int constraintComplex1(Graph *g, Node *node);
// void constraintComplex2 (Graph *g, Node *from, Node *to);

#endif

