%{
#include <stdio.h>
#include <stdlib.h>
#include "../include/Graph.h"
#include "../include/Parser_func.h"


extern int yylineno;
int yylex();

%}

%union {
    char* string;
    struct OpSeq* opseq; 
}

%token <string> ID
%token ASIGNACION REF DEREF
%type <opseq> statements statement block cond_skip

/* Tokens */
%token TLLAVE_OP TLLAVE_CL
%token TPAR_OP TPAR_CL
%token IF ELSE WHILE RETURN

%%

program: statements                { 
                                    Graph *graph = NULL; 
                                    eval_seq($1, &graph); 
                                    printDot(graph, "../output/grafo.dot");
                                    opSeq_destroy($1);
                                   }

statements: statements statement  { $$ = opSeq_concat($1, $2); }
           | statement            { $$ = $1; }
           ;

statement: 
    ID ASIGNACION REF ID ';' {
                                OpSeq *s=opSeq_new(); 
                                opSeq_push(s, op_base($1,$4)); 
                                printf("Base Constraint: %s ⊇ {%s}\n", $1, $4);
                                $$ = s;
                             }
    | ID ASIGNACION ID ';'   {
                                OpSeq *s = opSeq_new();
                                opSeq_push(s, op_simple($1,$3)); 
                                $$=s;

                                printf("Simple Constraint: %s ⊇ %s\n", $1, $3);
                             }
    | ID ASIGNACION DEREF ID ';' {
                                OpSeq *s = opSeq_new(); 
                                opSeq_push(s, op_c1($1,$4)); 
                                $$=s;
                                printf("Complex 1 Constraint: %s ⊇ *%s\n", $1, $4);
                             }
    | DEREF ID ASIGNACION ID ';' {
                                OpSeq *s = opSeq_new(); 
                                opSeq_push(s, op_c2($2,$4)); 
                                $$=s;
                                printf("Complex 2 Constraint: *%s ⊇ %s\n", $2, $4);
                             }

    | IF TPAR_OP cond_skip TPAR_CL block
      {
        OpSeq *s = opSeq_new();
        opSeq_push(s, op_if($5, NULL));
        $$ = s;
        printf("Op IF SOLO\n");
      }
    | IF TPAR_OP cond_skip TPAR_CL block ELSE block
      {
        OpSeq *s = opSeq_new();
        opSeq_push(s, op_if($5, $7));
        $$ = s;
        printf("Op IF y ELSE\n");
      }

    | WHILE TPAR_OP cond_skip TPAR_CL block
      {
        /* para reconocer la estructura */
      }

    /* Bloque (por si quiero anidar) */
    | block
      { 
        $$ = $1;
      }
    ;



block: 
    TLLAVE_OP statements TLLAVE_CL { $$ = $2; }
    | TLLAVE_OP TLLAVE_CL { $$ = opSeq_new(); }
    ;


/* Mejorar esta parte con IDs, *, &, =, !, <, >, etc. */
cond_skip
  : /* empty */                 { $$ = opSeq_new(); }
  | cond_skip ID                { $$ = $1; free($2); }
  | cond_skip DEREF             { $$ = $1; }
  | cond_skip REF               { $$ = $1; }
  | cond_skip ASIGNACION        { $$ = $1; }
;

%%