#include "../include/Parser_func.h"
#include "../include/Graph.h"
#include "../include/Adm.h"
#include "../include/Algorithms.h"

#include <string.h>

OpSeq* opSeq_new(void){
  OpSeq *s = (OpSeq*)malloc(sizeof(OpSeq));
  s->head = s->tail = NULL;
  return s;
}

void opSeq_push(OpSeq *s, Op *op){
    op->next = NULL;
    if(!s->tail) {
        s->head = op; 
        s->tail = op; 
    } else { 
        s->tail->next=op; 
        s->tail=op; 
    }
}

OpSeq* opSeq_concat(OpSeq *a, OpSeq *b){
    if (!a) return b;
    if (!a->head) { 
        a->head = b->head; 
        a->tail = b->tail; 
        free(b);          
        return a; 
    }
    a->tail->next = b->head;
    a->tail = b->tail; 
    free(b);
    return a;
}

static Op* new_op(OpKind k){ 
    Op *o = (Op *) malloc(sizeof(Op)); 
    o->kind=k; 
    o->a=o->b=NULL; 
    o->then_seq= NULL;
    o->else_seq=NULL; 
    o->next=NULL; 
    return o; 
}
Op* op_base(char *a, char *b){ 
    Op* o=new_op(OP_BASE); 
    o->a = a; 
    o->b = b; 
    return o; 
}
Op* op_simple(char *a, char *b){ 
    Op* o=new_op(OP_SIMPLE); 
    o->a=a; 
    o->b=b; 
    return o; 
}
Op* op_c1(char *l, char *r){ 
    Op* o=new_op(OP_C1); 
    o->a=l; 
    o->b=r; 
    return o; 
}
Op* op_c2(char *l, char *r){ 
    Op* o=new_op(OP_C2); 
    o->a=l; 
    o->b=r; 
    return o; 
}
Op* op_if(OpSeq *t, OpSeq *e){ 
    Op* o=new_op(OP_IF); 
    o->then_seq=t; 
    o->else_seq=e; 
    return o; 
}
Op* op_while(OpSeq *body){ 
    Op* o=new_op(OP_WHILE); 
    o->then_seq = body; 
    o->else_seq = NULL; 
    return o; 
}

void opSeq_destroy(OpSeq *s){
  for (Op *o= s->head, *next; o; o=next){
    next = o->next;
    if (o->kind==OP_IF){
        if (o->then_seq){ 
            opSeq_destroy(o->then_seq); 
            free(o->then_seq); 
        }
        if (o->else_seq){ 
            opSeq_destroy(o->else_seq); 
            free(o->else_seq); 
        }
    } else { 
        free(o->a); 
        free(o->b); 
    }
    free(o);
  }
  s->head= NULL;
  s->tail=NULL;
}


void eval_seq(OpSeq *seq, struct Graph **G){
  for(Op *op=seq->head; op; op=op->next){
    switch(op->kind){
        case OP_BASE:  {  
            constraitBase(G,   op->a, op->b); 
            break;
        }
        case OP_SIMPLE: { 
            constraintSimple(G, op->a, op->b); break;
        }
        case OP_C1: {    
            constraintComplex1(G, op->a, op->b); break;
        }
      case OP_C2: {    
            constraintComplex2(G, op->a, op->b); break;
        }

        case OP_IF: {
            Graph *base = *G;
            Graph *gThen = graph_clone(base);
            eval_seq(op->then_seq, &gThen);
            wave_Propagation(&gThen);
            if(op->else_seq) {
                //Rama del ELSE
                Graph *gElse = graph_clone(base);
                eval_seq(op->else_seq, &gElse);
                wave_Propagation(&gElse);

                Graph *J = graph_join(gThen, gElse);
                *G = J;
            } else {
                // if sin else
                Graph *J = graph_join(base, gThen);
                *G = J;
            }
            break;
        }

        case OP_WHILE: {
            Graph *base = *G;
            Graph *gwhile = graph_clone(base);

            //Mientras no cambie el grafo
                // evaluar op.then_seq en el grafo gwhile
                // ejecuto el algoritmo wp
            
            // Si sale del ciclo es porque ya el grafo no cambio con respecto al anterior.
            // Hago el join con el base
            

        }
    }
  }
  wave_Propagation(G);
}
