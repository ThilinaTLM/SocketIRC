
#ifndef SOCKETIRC_CMD_H
#define SOCKETIRC_CMD_H

#include <stdlib.h>
#include <string.h>

#define C_JOIN 0
#define C_NICK 1
#define C_WHO 2
#define C_WHOSIS 3
#define C_TIME 4
#define C_PRIVMSG 5
#define C_BCASTMSG 6
#define C_QUIT 7

#define COMMAND_LEN 50
#define ARG_LEN 100
#define LAST_ARG_LEN 300

struct CommandData {
    int code;
    char arg1[ARG_LEN];
    char arg2[LAST_ARG_LEN];
};

int cd_parse_code(int *code, const char * raw, int pointer) {
    char cmd[COMMAND_LEN];
    int ind = 0;
    char c = *(raw  + pointer + ind);
    while (c != ' ' && c != 0 && ind < COMMAND_LEN ) {
        cmd[ind] = c;
        c = *(raw  + pointer + (++ind));
    }
    cmd[ind] = 0; // break string

    if (strcmp((const char *) cmd, "JOIN") == 0) {
        *code = C_JOIN;
        return pointer + ind + 1;
    } else if (strcmp((const char *) cmd, "NICK") == 0) {
        *code = C_NICK;
        return pointer + ind + 1;
    } else if (strcmp((const char *) cmd, "WHO") == 0) {
        *code = C_WHO;
        return pointer + ind + 1;
    } else if (strcmp((const char *) cmd, "WHOSIS") == 0) {
        *code = C_WHOSIS;
        return pointer + ind + 1;
    } else if (strcmp((const char *) cmd, "TIME") == 0) {
        *code = C_TIME;
        return pointer + ind + 1;
    } else if (strcmp((const char *) cmd, "PRIVMSG") == 0) {
        *code = C_PRIVMSG;
        return pointer + ind + 1;
    } else if (strcmp((const char *) cmd, "BCASTMSG") == 0) {
        *code = C_BCASTMSG;
        return pointer + ind + 1;
    } else if (strcmp((const char *) cmd, "QUIT") == 0) {
        *code = C_QUIT;
        return pointer + ind + 1;
    }
    return -1;
}

int cd_parse_arg(char *arg, const char * raw, int pointer) {
    char frame[ARG_LEN];
    int ind = 0;
    char c = *(raw  + pointer + ind);
    while (c != ' ' && c != 0 && ind < ARG_LEN ) {
        frame[ind] = c;
        c = *(raw  + pointer + (++ind));
    }
    frame[ind] = 0; // break string
    strcpy( arg, frame);
    return pointer + ind + 1;
}

int cd_parse_arg_last(char *arg, const char * raw, int pointer) {
    char frame[LAST_ARG_LEN];
    int ind = 0;
    char c = *(raw  + pointer + ind);
    while (c != 0 && ind < LAST_ARG_LEN ) {
        frame[ind] = c;
        c = *(raw  + pointer + (++ind));
    }
    frame[ind] = 0; // break string
    strcpy( arg, frame);
    return pointer + ind + 1;
}

int cd_parse(struct CommandData *cd, const char * raw) {
    int pointer = 0;
    cd->code = -1;
    strcpy(cd->arg1, "");strcpy(cd->arg2, "");

    pointer = cd_parse_code(&cd->code, raw, pointer);
    if (pointer == -1) return  -1;


    pointer = cd_parse_arg(&cd->arg1[0], raw, pointer);
    if (pointer == -1) return  -1;
    pointer = cd_parse_arg_last(&cd->arg2[0], raw, pointer);
    if (pointer == -1) return  -1;
    return 0;
}

void cd_show(struct CommandData *cd) {
    printf("COMMAND CODE: %d\n", cd->code);
    printf("\tARG1: %s\n", cd->arg1);
    printf("\tARG2: %s\n", cd->arg2);
}

#endif //SOCKETIRC_CMD_H
