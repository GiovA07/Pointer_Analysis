
#ifndef  STATE_REC_H
#define  STATE_REC_H


#include <stdlib.h>

#include "./Graph.h"

typedef struct StateRec   StateRec;
typedef struct StateTable StateTable;

struct StateRec
{
  int op_id;
  Graph *in;
  Graph *out;
  struct StateRec *next;
};

struct StateTable{
    struct StateRec *head;
};

StateTable* state_table_create();

void state_set_in(StateTable *st, int id, Graph *gclone);
void state_set_out(StateTable *st, int id, Graph *gclone);

Graph* state_get_in(StateTable *st, int id);
Graph* state_get_out(StateTable *st, int id);

void state_destroy(StateTable **st); // destruye todos los grafos guardados

void state_menu(StateTable *state_tbl); // Menu utilizado para poder ver cada estado del programa

#endif