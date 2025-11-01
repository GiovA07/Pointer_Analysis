#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./Graph.h"
#include "./Node.h"
#include "./ListConstraint.h"

#ifndef ADM_H
#define ADM_H

// Funciones para los constraints
void constraitBase(Graph **g, char *nameNode1, char *nameNode2);
void constraintSimple(Graph **g, char *nameNode1, char *nameNode2);

void constraintComplex1(Graph **g, char *l_name, char *r_name);
void constraintComplex2(Graph **g, char *l_name, char *r_name);

#endif

