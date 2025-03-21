#include "../include/Adm.h"

void constraintBase(Node* node, Node* ref) {
    addReference(node, ref);
}

void constraintSimple(Graph *g, Node* toEdge) {
    addEdge(g, toEdge);
}
