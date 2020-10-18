
#ifndef SOCKETIRC_CPROFILE_H
#define SOCKETIRC_CPROFILE_H

#include <string.h>
#include <time.h>
#include <pthread.h>

#define MSG_LENGTH 1024

#define STATUS_DISCONNECTED 0
#define STATUS_CONNECTED 1
#define STATUS_JOINED 2
#define STATUS_DEAD 3

/**
 * Client Profile
 * Includes client specific data
 */
struct ClientData {
    pthread_t thread;
    int status;
    int socket_fd;
    char *ip_address;
    char *nickname;
    char *realname;
    char *username;
    time_t joined_time;
    pthread_mutex_t send_lock;
};

struct ClientData *cp_create();

void cp_free(struct ClientData *cp);

void cp_set_name(struct ClientData *cp, const char *realname, const char *username);

void cp_set_nickname(struct ClientData *cp, const char *nickname);

void cp_set_joined_tme(struct ClientData *cp);

long cp_get_duration(struct ClientData *cp);

char *cp_to_string(struct ClientData *cp);

void cp_send_message(struct ClientData *cp, const char *msg);

void cp_show(struct ClientData *cp);

/**
 *
 * @param socket_fd : Socket File Descriptor
 * @param ip_address : Socket IP Address
 * @return : Client Profile Struct
 */
struct ClientData* cp_create() {
    struct ClientData *cp = malloc(sizeof(struct ClientData));
    cp->ip_address = NULL;
    cp->nickname = NULL;
    cp->realname = NULL;
    cp->username = NULL;
    return cp;
}

void cp_clean(struct ClientData *cp) {
    if (cp->ip_address != NULL) free(cp->ip_address);
    if (cp->username != NULL) free(cp->username);
    if (cp->nickname != NULL) free(cp->nickname);
    if (cp->realname != NULL) free(cp->realname);
    cp->socket_fd = 0;
    cp->joined_time = 0;
}

void cp_free(struct ClientData *cp) {
    if (cp->ip_address != NULL) free(cp->ip_address);
    if (cp->username != NULL) free(cp->username);
    if (cp->nickname != NULL) free(cp->nickname);
    if (cp->realname != NULL) free(cp->realname);
    free(cp);
}

void cp_set_ip_address(struct ClientData *cp, const char *ip_address) {
    cp->ip_address = malloc(sizeof(char) * strlen(ip_address));
    strcpy(cp->ip_address, ip_address);
}

void cp_set_name(struct ClientData *cp, const char *realname, const char *username) {
    cp->realname = malloc(sizeof(char) * strlen(realname));
    cp->username = malloc(sizeof(char) * strlen(realname));
    strcpy(cp->realname, realname);
    strcpy(cp->username, username);
}

void cp_set_nickname(struct ClientData *cp, const char *nickname) {
    if (cp->nickname != NULL)
        cp->nickname = realloc(cp->nickname, sizeof(char) * strlen(nickname));
    else
        cp->nickname = malloc(sizeof(char) * strlen(nickname));
    strcpy(cp->nickname, nickname);
}

void cp_set_joined_tme(struct ClientData *cp) {
    cp->joined_time = time(NULL);
}

void cp_send_message(struct ClientData *cp, const char *msg) {
    pthread_mutex_lock(&cp->send_lock);
    send(cp->socket_fd, msg, MSG_LENGTH, 0);
    pthread_mutex_unlock(&cp->send_lock);
}


long cp_get_duration(struct ClientData *cp) {
    return time(0) - cp->joined_time;
}

char * cp_get_nick_or_username(struct ClientData *cp) {
    if (cp->nickname != NULL) {
        return cp->nickname;
    } else {
        return cp->username;
    }
}

char *cp_to_string(struct ClientData *cp) {
    if (cp == NULL) {
        return "";
    }

    const char * nickname;
    if (cp->nickname != NULL) {
        nickname = cp->nickname;
    } else {
        nickname = "NONE";
    }

    char *buffer = malloc(MSG_LENGTH * sizeof(char));
    sprintf(buffer,
            "%s | NICKNAME: '%s' | REALNAME: '%s' | IP: %s | TIME: %ld sec",
            cp->username, nickname, cp->realname, cp->ip_address, cp_get_duration(cp)
    );
    return buffer;
}

void cp_show(struct ClientData *cp) {
    if (cp == NULL) {
        return;
    }
    printf("Client Data\n");
    printf("\tusername: %s\n", cp->username);
    printf("\trealname: %s\n", cp->realname);
    printf("\tnickname: %s\n", cp->nickname);
}


#endif //SOCKETIRC_CPROFILE_H
