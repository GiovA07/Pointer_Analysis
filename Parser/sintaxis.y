%{
#include <stdio.h>
#include <stdlib.h>
#include "../include/Algorithms.h"
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
%type <graph> statements statement block
/* es un skip muy simple para ignorar lo que haya entre parentesis en una condicion */
%type <graph> cond_skip

/* Tokens */
%token TLLAVE_OP TLLAVE_CL
%token TPAR_OP TPAR_CL
%token IF ELSE WHILE RETURN






%%

program: statements                { 
                                    wave_Propagation(&graph);
                                    printDot(graph, "../output/grafo.dot");
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

                                Graph* aux = findNode(graph, n1->name);
                                Graph* aux2 = findNode(graph, n2->name);
                                n1 = aux->node;
                                n2 = aux2->node;

                                addConstraint(&listComplex1, n1, n2);

                                printf("Complex 1 Constraint: %s ⊇ *%s\n", $1, $4);
                                free($1); free($4);
                             }
    | DEREF ID ASIGNACION ID ';' {
                                Node *n1 = createNode($2);
                                Node *n2 = createNode($4);

                                addNode(&graph, n1);
                                addNode(&graph, n2);

                                Graph* aux = findNode(graph, n1->name);
                                Graph* aux2 = findNode(graph, n2->name);
                                n1 = aux->node;
                                n2 = aux2->node;

                                addConstraint(&listComplex2, n1, n2);

                                printf("Complex 2 Constraint: *%s ⊇ %s\n", $2, $4);
                                free($2); free($4);
                             }

    | IF TPAR_OP cond_skip TPAR_CL block
      {
        /* para reconocer la estructura */
      }
    | IF TPAR_OP cond_skip TPAR_CL block ELSE block
      {
        /* para reconocer la estructura */
      }

    | WHILE TPAR_OP cond_skip TPAR_CL block
      {
        /* para reconocer la estructura */
      }

    /* Bloque (por si quiero anidar) */
    | block
      { 
        /* sin accion */ 
      }
    ;



block: 
    TLLAVE_OP statements TLLAVE_CL { $$ = graph; }
    | TLLAVE_OP TLLAVE_CL { $$ = graph; }
    ;


/* Mejorar esta parte con IDs, *, &, =, !, <, >, etc. */
cond_skip
    : /* empty */                         { $$ = graph; }
    | cond_skip ID                        { $$ = graph; free($2); }
    | cond_skip DEREF                     { $$ = graph; }
    | cond_skip REF                       { $$ = graph; }
    | cond_skip ASIGNACION                { $$ = graph; }
    ;

%%