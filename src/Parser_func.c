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
        case OP_BASE:   constraitBase(G,   op->a, op->b); break;
        case OP_SIMPLE: constraintSimple(G, op->a, op->b); break;
        case OP_C1:     constraintComplex1(G, op->a, op->b); break;
        case OP_C2:     constraintComplex2(G, op->a, op->b); break;

        case OP_IF: {
            printf("[Operator] IF\n");
            Graph *base = *G;
            Graph *gThen = graph_clone(base);
            eval_seq(op->then_seq, &gThen);
            wave_Propagation(&gThen);
            if(op->else_seq) {
                printf("[Operator] ELSE\n");
                //Rama del ELSE
                Graph *gElse = graph_clone(base);
                eval_seq(op->else_seq, &gElse);
                wave_Propagation(&gElse);

                Graph *J = graph_join(gThen, gElse);
                *G = J;
                /*TODO: destruir gthen/gelse*/
            } else {
                // if sin else
                Graph *J = graph_join(base, gThen);
                *G = J;
                /*TODO: destruir gthen*/
            }
            break;
        }

        case OP_WHILE: {
            printf("[Operator] WHILE\n");
            Graph *gaux = graph_clone(*G);
            int maxwhile = 10;
            while (maxwhile > 0) {
                Graph *prev = graph_clone(gaux);

                eval_seq(op->then_seq, &gaux);
                Graph *join = graph_join(prev, gaux);
                if (graphs_equal(join, prev)) {
                    *G = join;
                    // TODO: graph_destroy(prev);
                    // TODO: graph_destroy(gaux); // si join != gaux
                    break;
                }
                //Sino sigo ejecutando el while
                // TODO: graph_destroy(prev);
                // TODO: graph_destroy(gaux);
                gaux = join;
                maxwhile = maxwhile - 1;
            }
            if (maxwhile <= 0) {
                *G = gaux;
                printf("Se hizo el mayor ciclo posible \n");
            }
            break;
        }
    }
  }
  wave_Propagation(G);
}
