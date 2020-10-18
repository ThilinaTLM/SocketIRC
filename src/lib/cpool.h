//
// Created by tlm on 10/16/20.
//

#ifndef SOCKETIRC_CPOOL_H
#define SOCKETIRC_CPOOL_H

#include "cprofile.h"

struct Node {
    struct ClientData *profile;
    struct Node *next;
    struct Node *prev;
};

struct ClientPool {
    int max_size;
    int size;
    struct Node *head;
};

struct ClientPool *cpool_create(int max_size);

int cpool_add(struct ClientPool *cpool, struct ClientData *cp);

struct ClientData *cpool_get(struct ClientPool *cpool, int index);

//struct ClientData *cpool_pop(struct ClientPool *cpool, int index);

struct ClientData *cpool_find_byNickname(struct ClientPool *cpool, const char *nickname);

struct ClientData *cpool_find_byUsername(struct ClientPool *cpool, const char *username);

// ---------------------------------------------------------------------------------------------------------------------


struct ClientPool *cpool_create(int max_size) {
    struct ClientPool *cp = malloc(sizeof(struct ClientPool));
    cp->size = 0;
    cp->max_size = max_size;
    cp->head = NULL;
    return cp;
}

struct ClientData *cpool_get(struct ClientPool *cpool, int index) {
    if (index >= cpool->size) {
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

int cpool_add(struct ClientPool *cpool, struct ClientData *cp) {
    if (cpool->max_size <= cpool->size) {
        return -1;
    }

    struct Node *n = malloc(sizeof(struct Node));
    n->profile = cp;
    n->next = NULL;
    n->prev = NULL;

    if (cpool->size == 0) {
        cpool->head = n;
        cpool->size++;
        return 0;
    }

    n->next = cpool->head;
    cpool->head->prev = n;
    cpool->head = n;
    cpool->size++;
    return 0;
}

//struct ClientData *cpool_pop(struct ClientPool *cpool, int index) {
//    if (index >= cpool->size) {
//        return NULL;
//    }
//    struct Node *n = cpool->head;
//    while (index > 0) {
//        n = n->next;
//        index--;
//    }
//
//    if (n->next != NULL)
//        n->next->prev = n->prev;
//    if (n->prev != NULL)
//        n->prev->next = n->next;
//    cpool->size--;
//    struct ClientData *cp = n->profile;
//    free(n);
//    return cp;
//}

struct ClientData *cpool_find_byNickname(struct ClientPool *cpool, const char *nickname) {
    if (nickname == NULL) return NULL;
    struct ClientData *cp;
    for (int i = 0; i < cpool->size; i++) {
        cp = cpool_get(cpool, i);
        if (cp->nickname != NULL && strcmp(cp->nickname, nickname) == 0) {
            return cp;
        }
    }
    return NULL;
}

struct ClientData *cpool_find_byUsername(struct ClientPool *cpool, const char *username) {
    if (username == NULL) return NULL;
    struct ClientData *cp;
    for (int i = 0; i < cpool->size; i++) {
        cp = cpool_get(cpool, i);
        if (cp->username != NULL && strcmp(cp->username, username) == 0) {
            return cp;
        }
    }
    return NULL;
}


#endif //SOCKETIRC_CPOOL_H
