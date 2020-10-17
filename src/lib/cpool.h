//
// Created by tlm on 10/16/20.
//

#ifndef SOCKETIRC_CPOOL_H
#define SOCKETIRC_CPOOL_H

#include "cprofile.h"

struct Node {
    struct ClientProfile *profile;
    struct Node *next;
    struct Node *prev;
};

struct ClientPool {
    int size;
    int count;
    struct Node *head;
};

struct ClientProfile* cpool_find_byNickname(struct ClientPool *cpool, const char* nickname) {
    struct Node *n = cpool->head;
    int depth = cpool->count;
    while (depth > 0) {
        if (strcmp(n->profile->nickname, nickname) == 0) {
            return n->profile;
        }
        depth--;
        n = n->next;
    }
    return NULL;
}

struct ClientProfile* cpool_get(struct ClientPool *cpool, int index) {
    if (index >= cpool->count) {
        return NULL;
    }

    struct Node *n;
    n = cpool->head;
    while (index > 0) {
        n = n->next;
        index--;
    }
    return n->profile;
}

int cpool_add(struct ClientPool *cpool, struct ClientProfile *cp) {
    if (cpool->size <= cpool->count) {
        return -1;
    }

    struct Node n;
    n.profile = cp;
    n.next = NULL;
    n.prev = NULL;

    if (cpool->count == 0) {
        cpool->head = &n;
        cpool->count++;
        return 0;
    }


    n.next = cpool->head;
    cpool->head->prev = &n;
    cpool->head = &n;
    cpool->count++;
    return 0;
}

struct ClientProfile* cpool_pop_byNickname(struct ClientPool *cpool, const char* nickname) {
    int depth = cpool->count;
    struct Node *n;
    n = cpool->head;
    while (depth > 0) {
        if (strcmp(n->profile->nickname, nickname) == 0) {
            n->next->prev = n->prev;
            n->prev->next = n->next;
            return n -> profile;
        }
        n = n->next;
        depth--;
    }
    return NULL;
}


#endif //SOCKETIRC_CPOOL_H
