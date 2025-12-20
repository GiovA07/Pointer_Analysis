#include "../include/Alias.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct AliasMap {
    char *from;
    char *to;
    struct AliasMap *next;
} AliasMap;

static AliasMap *AMap = NULL;

static AliasMap* find_entry(char *from) {
    for (AliasMap *m = AMap; m; m = m->next)
        if (strcmp(m->from, from) == 0) return m;
    return NULL;
}

char* alias_find(char *name) {
    AliasMap *m = find_entry(name);
    if (!m) return name;
    char *rep = alias_find(m->to);
    if (rep != m->to) {
        free(m->to);
        m->to = strdup(rep);
    }
    return m->to;
}

void alias_bind(char *from, char *to) {
    char *rep_to = alias_find(to); // siempre bind al representante final
    AliasMap *m = find_entry(from);
    if (m) {
        free(m->to);
        m->to = strdup(rep_to);
        return;
    }
    m = (AliasMap*)malloc(sizeof(AliasMap));
    m->from = strdup(from);
    m->to   = strdup(rep_to);
    m->next = AMap;
    AMap    = m;
}