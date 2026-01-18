#include "../include/Parser_func.h"

#include <string.h>

int global_id_op = 1;

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


static void wp_with_reset(Graph **G) {
    // invalidamos las caches ya que se ejecutara wave en cada sentencia
    constraints_reset_all_caches(listComplex1);
    constraints_reset_all_caches(listComplex2);

    wave_Propagation(G);
}

static void eval_stmt(Graph **G, Op *s, StateTable *st) {
    switch (s->kind) {
        case OP_BASE:       constraitBase(G, s->a, s->b); break;    // a = &b
        case OP_SIMPLE:     constraintSimple(G, s->a, s->b); break; // a = b
        case OP_C1:         constraintComplex1(G, s->a, s->b); break; // a = *b
        case OP_C2:         constraintComplex2(G, s->a, s->b); break; // *a = b
        case OP_IF: {
            printf("[Operator] IF\n");
            Graph *base = *G;

            Graph *gThen = graph_clone(base);
            eval_seq(s->then_seq, &gThen, st);
            Graph *J = NULL;

            if(s->else_seq) {
                printf("[Operator] ELSE\n");

                Graph *gElse = graph_clone(base);
                eval_seq(s->else_seq, &gElse, st);

                J = graph_join(gThen, gElse);

                graph_destroy(&gElse);
            } else {
                // if sin else
                J = graph_join(base, gThen);
            }
            graph_destroy(&gThen);
            // reemplazo IN por OUT
            graph_destroy(&base);
            *G = J;
            break;
        }

        case OP_WHILE: {
            printf("[Operator] WHILE\n");
            Graph *IN0 = graph_clone(*G);
            Graph *gaux = graph_clone(*G);

            int maxwhile = 10;
            while (maxwhile-- > 0) {
                Graph *bodyOUT = graph_clone(gaux);
                eval_seq(s->then_seq, &bodyOUT, st);
                
                Graph *join = graph_join(IN0, bodyOUT);
                wp_with_reset(&join);
                graph_destroy(&bodyOUT);
                if (graphs_equal(join, gaux)) {
                    graph_destroy(&gaux);
                    gaux = join;
                    break;
                }
                //Sino sigo ejecutando el while
                graph_destroy(&gaux);
                gaux = join;
            }
            graph_destroy(&IN0);
            graph_destroy(G);
            *G = gaux;
            break;
        }
    }
}


static Graph* step_stmt(Graph *gIN, Op *op, StateTable *st) {
    int id = global_id_op++;
    state_set_in(st, id, graph_clone(gIN));

    Graph *gOUT = graph_clone(gIN);
    eval_stmt(&gOUT, op, st);
    wp_with_reset(&gOUT);        // o wave_Propagation(&gout)

    state_set_out(st, id, graph_clone(gOUT));

    return gOUT;
}


void eval_seq(OpSeq *seq, struct Graph **G, StateTable *st) {
    Graph *state = *G;  // estado actual (IN)
    for (Op *it = seq->head; it; it = it->next) {
        Graph *next = step_stmt(state, it, st);

        graph_destroy(&state);
        state = next;
    }
    *G = state; // estado final
}
