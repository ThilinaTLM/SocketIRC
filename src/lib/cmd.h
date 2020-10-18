
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

#define COMMAND_CODE_LEN 50
#define ARG_LEN 100
#define LAST_ARG_LEN 300

struct CommandData {
    int code;
    char arg1[ARG_LEN];
    char arg2[LAST_ARG_LEN];
};

int skip_trailing_spaces(const char *raw, int pointer) {
    int ind = 0;
    char c = *(raw + pointer + ind);
    while (c == ' ') c = *(raw + pointer + (++ind));
    if (c == '\n' || c == 0) {
        return -1;
    }
    return pointer + ind;
}

int cd_parse_code(int *code, const char * raw, int pointer) {
    pointer = skip_trailing_spaces(raw, pointer);
    if (pointer == -1) {
        return -1;
    }

    char cmd[COMMAND_CODE_LEN];
    int ind = 0;
    char c = *(raw  + pointer + ind);
    while (c != ' ' && c != 0 && ind < COMMAND_CODE_LEN ) {
        cmd[ind] = c;
        c = *(raw  + pointer + (++ind));
    }
    cmd[ind] = 0; // break string

    if (strcasecmp((const char *) cmd, "JOIN") == 0) {
        *code = C_JOIN;
        return pointer + ind;
    } else if (strcasecmp((const char *) cmd, "NICK") == 0) {
        *code = C_NICK;
        return pointer + ind;
    } else if (strcasecmp((const char *) cmd, "WHO") == 0) {
        *code = C_WHO;
        return pointer + ind;
    } else if (strcasecmp((const char *) cmd, "WHOSIS") == 0) {
        *code = C_WHOSIS;
        return pointer + ind;
    } else if (strcasecmp((const char *) cmd, "TIME") == 0) {
        *code = C_TIME;
        return pointer + ind;
    } else if (strcasecmp((const char *) cmd, "PRIVMSG") == 0) {
        *code = C_PRIVMSG;
        return pointer + ind;
    } else if (strcasecmp((const char *) cmd, "BCASTMSG") == 0) {
        *code = C_BCASTMSG;
        return pointer + ind;
    } else if (strcasecmp((const char *) cmd, "QUIT") == 0) {
        *code = C_QUIT;
        return pointer + ind;
    }
    return -1;
}

int cd_parse_arg(char *arg, const char * raw, int pointer) {
    pointer = skip_trailing_spaces(raw, pointer);
    if (pointer == -1) {
        return -1;
    }

    char frame[ARG_LEN];
    int ind = 0;
    char c = *(raw  + pointer + ind);
    while (c != ' ' && c != 0 && ind < ARG_LEN ) {
        frame[ind] = c;
        c = *(raw  + pointer + (++ind));
    }
    frame[ind] = 0; // break string
    strcpy( arg, frame);
    return pointer + ind;
}

int cd_parse_arg_last(char *arg, const char * raw, int pointer) {
    pointer = skip_trailing_spaces(raw, pointer);
    if (pointer == -1) {
        return -1;
    }

    char frame[LAST_ARG_LEN];
    int ind = 0;
    char c = *(raw  + pointer + ind);
    while (c != 0 && ind < LAST_ARG_LEN ) {
        frame[ind] = c;
        c = *(raw  + pointer + (++ind));
    }
    frame[ind] = 0; // break string
    strcpy( arg, frame);
    return pointer + ind;
}

int cd_parse(struct CommandData *cd, const char * raw) {
    int pointer = 0;
    cd->code = -1;
    strcpy(cd->arg1, "");strcpy(cd->arg2, "");

    pointer = cd_parse_code(&cd->code, raw, pointer);
    if (pointer == -1) return  -1;

    if (cd->code == C_TIME || cd->code == C_WHO) {
        return 0;
    }

    pointer = cd_parse_arg(&cd->arg1[0], raw, pointer);
    if (pointer == -1) return  -1;

    if (cd->code == C_NICK || cd->code == C_WHOSIS || cd->code == C_BCASTMSG || cd->code == C_QUIT) {
        return 0;
    }

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
