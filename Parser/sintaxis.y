%{
#include <stdio.h>
#include <stdlib.h>
#include "../include/Algorithms.h"
#include "../include/ListConstraint.h"
#include "../include/Graph.h"
#include "../include/Adm.h"

Graph* graph = NULL;

extern int yylineno;
int yylex();

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
                                constraitBase(&graph, $1, $4);
                                printf("Base Constraint: %s ⊇ {%s}\n", $1, $4);
                                free($1); free($4);
                                $$ = graph;
                             }
    | ID ASIGNACION ID ';'   {
                                constraintSimple(&graph, $1, $3);
                                $$ = graph;

                                printf("Simple Constraint: %s ⊇ %s\n", $1, $3);
                                free($1); free($3);
                             }
    | ID ASIGNACION DEREF ID ';' {
                                constraintComplex1(&graph, $1, $4);
                                printf("Complex 1 Constraint: %s ⊇ *%s\n", $1, $4);
                                free($1); free($4);
                             }
    | DEREF ID ASIGNACION ID ';' {
                                constraintComplex2(&graph, $2, $4);
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