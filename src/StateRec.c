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

    r->in = gclone;
}

void state_set_out(StateTable *st, int id, Graph *gclone) {
    if (!st) return;

    StateRec *r = state_ensure(st, id);
    if (!r) return;

    r->out = gclone;
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
        free(it);
        it = next;
    }

    free(curSt);
    *st = NULL;
}


void state_menu(StateTable *state_tbl) {
    if (!state_tbl || !state_tbl->head) {
        printf("[Menu] No hay estados guardados.\n");
        return;
    }

    printf("\n=== STATE MENU ===\n");
    printf("Comandos:\n");
    printf("  <id> elegi id\n");
    printf("  q    salir\n");

    for (;;) {
        printf("\n<ID>: ");
        char buf[64];
        if (!fgets(buf, sizeof(buf), stdin)) break;

        if (buf[0] == 'q' || buf[0] == 'Q') break;

        int id = 0;
        if (sscanf(buf, "%d", &id) != 1) {
            printf("ID invalido.\n");
            continue;
        }

        Graph *gin  = state_get_in(state_tbl, id);
        Graph *gout = state_get_out(state_tbl, id);

        if (!gin && !gout) {
            printf("No existe el id %d.\n", id);
            continue;
        }

        printf("Mostrar (i) in / (o) out / (b) both / (c)ancel ? ");
        if (!fgets(buf, sizeof(buf), stdin)) break;

        char k = (char)tolower((char)buf[0]);
        if (k == 'c') continue;

        if (k == 'i' || k == 'b') {
            if (!gin) printf("IN es NULL para id %d.\n", id);
            else {
                char fn[256];
                sprintf(fn, "state_in_%d.dot", id);
                printDot(gin, fn);
                printf("OK: %s\n", fn);
            }
        }

        if (k == 'o' || k == 'b') {
            if (!gout) printf("OUT es NULL para id %d.\n", id);
            else {
                char fn[256];
                sprintf(fn, "state_out_%d.dot", id);
                printDot(gout, fn);
                printf("OK: %s\n", fn);
            }
        }
    }
}