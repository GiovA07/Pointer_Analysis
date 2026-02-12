#ifndef PARSER_FUNC_H
#define PARSER_FUNC_H


  #include <stdlib.h>
  typedef struct Graph Graph;
  typedef struct StateTable StateTable;

  typedef enum { OP_BASE, OP_SIMPLE, OP_C1, OP_C2, OP_IF, OP_WHILE} OpKind;
  typedef struct Op   Op;
  typedef struct OpSeq OpSeq;

  struct Op {
    OpKind kind;
    char *a, *b;          // para BASE/SIMPLE/C1/C2
    OpSeq *then_seq;      // IF usa then_seq (para las operaciones del cuerpo) y para while.
    OpSeq *else_seq;      // IF: else opcional
    Op   *next;
  };
  struct OpSeq { Op *head, *tail; };

  /*helpers */
  OpSeq* opSeq_new();
  void opSeq_push(OpSeq *s, Op *op);
  OpSeq* opSeq_concat(OpSeq *a, OpSeq *b); /* concatena dos sequencias y devuelve a*/
  void opSeq_destroy(OpSeq *s);

  /*constructores de operaciones*/
  Op* op_base(char *a, char *b);
  Op* op_simple(char *a, char *b);
  Op* op_c1(char *l, char *r);
  Op* op_c2(char *l, char *r);
  Op* op_if(OpSeq *thenp, OpSeq *elsep);
  Op* op_while(OpSeq *body);

  /* Funcion para evaluar la lista de operaciones */
  void eval_seq(OpSeq *seq, struct Graph **G, StateTable *st);

  #endif
