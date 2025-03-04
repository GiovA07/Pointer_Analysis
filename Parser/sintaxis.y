%{
#include <stdio.h>
#include <stdlib.h>
#include "../include/Node.h"
#include "../include/ListNode.h"
#include "../include/Graph.h"
#include "../include/Adm.h"

Graph* graph = NULL;
Graph* mergeGraphs(Graph* g1, Graph* g2);

extern int yylineno;
int yylex();

ListVar* varList;

%}

%union {
    char* string;
    struct Graph* graph;

}

%token <string> ID
%token ASIGNACION REF DEREF
%type <graph> statements statement


%%

program: statements                { 
                                    printGraph(graph);
                                    printDot(graph, "../output/grafo.dot");
                                   }

statements: statements statement
           | statement           
           ;

statement: 
    ID ASIGNACION REF ID ';' {
                                Node *n1 = createNode($1);
                                Node *n2 = createNode($4);
                                addVar(&varList, n1);
                                addVar(&varList, n2);

                                addNode(&graph, n1);
                                addNode(&graph, n2);

                                Graph* aux = findNode(graph, n1->name);
                                Graph* aux2 = findNode(graph, n2->name);
                                
                                n1 = aux->node;
                                n2 = aux2->node;
                                constraintBase(n1, n2);

                                $$ = graph;
                                printf("Base Constraint: %s ⊇ {%s}\n", $1, $4);
                                free($1); free($4);
                             }
    | ID ASIGNACION ID ';'   {
                                Node *n1 = createNode($1);
                                Node *n2 = createNode($3);
                                addVar(&varList, n1);
                                addVar(&varList, n2);

                                addNode(&graph, n1);
                                addNode(&graph, n2);

                                Graph* aux = findNode(graph, n1->name);
                                Graph* aux2 = findNode(graph, n2->name);

                                n1 = aux->node;
                                constraintSimple(aux2, n1);

                                $$ = graph;

                                printf("Simple Constraint: %s ⊇ %s\n", $1, $3);
                                free($1); free($3);
                             }
    | ID ASIGNACION DEREF ID ';' {
                                Node *n1 = createNode($1);
                                Node *n2 = createNode($4);
                                addVar(&varList, n1);
                                addVar(&varList, n2);
                                printf("Complex 1 Constraint: %s ⊇ *%s\n", $1, $4);
                                free($1); free($4);
                             }
    | DEREF ID ASIGNACION ID ';' {
                                Node *n1 = createNode($2);
                                Node *n2 = createNode($4);
                                addVar(&varList, n1);
                                addVar(&varList, n2);
                                printf("Complex 2 Constraint: *%s ⊇ %s\n", $2, $4);
                                free($2); free($4);
                             }
    ;

%%

Graph* mergeGraphs(Graph* g1, Graph* g2) {
    if (!g1) return g2; // Si g1 es NULL, devolvemos g2
    if (!g2) return g1; // Si g2 es NULL, devolvemos g1

    Graph* temp = g1;
    while (temp->next) { // Recorremos hasta el último nodo
        temp = temp->next;
    }
    temp->next = g2; // Conectamos el último nodo de g1 con g2
    return g1; // Retornamos la lista combinada
}
