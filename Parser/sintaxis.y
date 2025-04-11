%{
#include <stdio.h>
#include <stdlib.h>
#include "../include/Node.h"
#include "../include/ListConstraint.h"
#include "../include/Graph.h"
#include "../include/Adm.h"

Graph* graph = NULL;

extern int yylineno;
int yylex();

ListConstraint* listComplex1;
ListConstraint* listComplex2;

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
                                    printConstraintComplex1(listComplex1);
                                    printConstraintComplex2(listComplex2);
                                   }

statements: statements statement
           | statement           
           ;

statement: 
    ID ASIGNACION REF ID ';' {
                                Node *n1 = createNode($1);
                                Node *n2 = createNode($4);

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

                                addNode(&graph, n1);
                                addNode(&graph, n2);

                                addConstraint(&listComplex1, n1, n2);

                                printf("Complex 1 Constraint: %s ⊇ *%s\n", $1, $4);
                                free($1); free($4);
                             }
    | DEREF ID ASIGNACION ID ';' {
                                Node *n1 = createNode($2);
                                Node *n2 = createNode($4);

                                addNode(&graph, n1);
                                addNode(&graph, n2);

                                addConstraint(&listComplex2, n1, n2);

                                printf("Complex 2 Constraint: *%s ⊇ %s\n", $2, $4);
                                free($2); free($4);
                             }
    ;

%%