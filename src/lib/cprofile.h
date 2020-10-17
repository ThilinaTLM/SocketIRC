//
// Created by tlm on 10/16/20.
//

#ifndef SOCKETIRC_CPROFILE_H
#define SOCKETIRC_CPROFILE_H

#include <string.h>
#include <time.h>

struct ClientProfile {
    int socket_fd;
    const char* nickname;
    const char* realname;
    const char* username;
    time_t joined_time;
};


void cp_set_name(struct ClientProfile *cp, const char *realname, const char *username) {
    cp->realname = realname;
    cp->username = username;
    cp->nickname = username;
}

void cp_set_nickname(struct ClientProfile *cp, const char *nickname) {
    cp->nickname = nickname;
}

long cp_get_duration(struct ClientProfile *cp) {
    return time(0) - cp->joined_time;
}

#endif //SOCKETIRC_CPROFILE_H
