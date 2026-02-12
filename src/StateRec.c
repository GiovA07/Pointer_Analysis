#include "../include/StateRec.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h> 

static StateRec* state_find(StateTable *st, int id) {
    for (StateRec *r = st->head; r; r = r->next) {
        if (r->op_id == id) return r;
    }
    return NULL;
}

static StateRec* state_ensure(StateTable *st, int id) {
    StateRec *r = state_find(st, id);
    if (r) return r;

    r = (StateRec*)malloc(sizeof(StateRec));
    if (!r) return NULL;

    r->op_id = id;
    r->in = NULL;
    r->out = NULL;
    r->data = NULL;

    r->next = st->head;
    st->head = r;
    return r;
}


StateTable* state_table_create() {
    StateTable *st = (StateTable*)malloc(sizeof(StateTable));
    if (!st) return NULL;
    st->head = NULL;
    return st;
}

void state_set_in(StateTable *st, int id, Graph *gclone) {
    if (!st) return;

    StateRec *r = state_ensure(st, id);
    if (!r) return;
    
    if (r->in) graph_destroy(&r->in);
    r->in = gclone;
}

void state_set_out(StateTable *st, int id, Graph *gclone) {
    if (!st) return;

    StateRec *r = state_ensure(st, id);
    if (!r) return;

    if (r->out) graph_destroy(&r->out);
    r->out = gclone;
}


void state_set_data(StateTable *st, int id, char *data){
   if (!st) return;

    StateRec *r = state_ensure(st, id);
    if (!r) { 
        free(data); 
        return; 
    }

    if (r->data) 
        free(r->data); 
    r->data = data;
}

Graph* state_get_in(StateTable *st, int id) {
    StateRec *r = state_find(st, id);
    if(r) return r->in;
    return NULL;
}

Graph* state_get_out(StateTable *st, int id) {
    StateRec *r = state_find(st, id);
    if(r) return r->out;
    return NULL;
}

void state_destroy(StateTable **st) {
    if (!st || !*st) return;

    StateTable *curSt = *st;
    StateRec *it = curSt->head;

    while(it)
    {
        StateRec *next = it->next;
        graph_destroy(&it->in);
        graph_destroy(&it->out);
        free(it->data);
        free(it);
        it = next;
    }

    free(curSt);
    *st = NULL;
}


static void state_print_table(StateTable *st) {
    printf("\nID   IN  OUT  Statement\n");
    printf("------------------------------------------\n");

    for (StateRec *r = st->head; r; r = r->next) {
        printf("%d  %s %s   %s\n",
            r->op_id,
            (r->in  ? "Si" : "No"),
            (r->out ? "Si" : "No"),
            (r->data ? r->data : "<sin data>")
        );
    }
}

void state_menu(StateTable *state_tbl) {
    if (!state_tbl || !state_tbl->head) {
        printf("[Menu] No hay estados guardados.\n");
        return;
    }

    printf("\n=== STATE MENU ===\n");
    printf("Comandos:\n");
    printf("  l    listar ids\n");
    printf("  <id> elegir id\n");
    printf("  q    salir\n");

    char buf[64];

    for (;;) {
        printf("\n-> ");
        if (!fgets(buf, sizeof(buf), stdin)) break;

        if (buf[0] == 'q' || buf[0] == 'Q') break;

        if (buf[0] == 'l' || buf[0] == 'L' || buf[0] == 0) {
            state_print_table(state_tbl);
            continue;
        }

        int id = 0;
        if (sscanf(buf, "%d", &id) != 1) {
            printf("Comando invalido.\n");
            continue;
        }

        StateRec *r = state_find(state_tbl, id);
        if (!r) {
            printf("No existe el id %d.\n", id);
            continue;
        }

        printf("ID %d: %s\n", id, r->data ? r->data : "<sin data>");

        printf("Elije (i) in / (o) out / (b) both / (c) cancelar ");
        if (!fgets(buf, sizeof(buf), stdin)) break;

        char k = (char)tolower((char)buf[0]);
        if (k == 'c') continue;

        if (k == 'i' || k == 'b') {
            if (!r->in) printf("IN es NULL para id %d.\n", id);
            else {
                char fn[256];
                sprintf(fn, "graph_IN_%d.dot", id);
                printDot(r->in, fn);
                printf("Se guardo en: %s\n", fn);
            }
        }

        if (k == 'o' || k == 'b') {
            if (!r->out) printf("OUT es NULL para id %d.\n", id);
            else {
                char fn[256];
                sprintf(fn, "graph_OUT_%d.dot", id);
                printDot(r->out, fn);
                printf("Se guardo en: %s\n", fn);
            }
        }
    }
}
