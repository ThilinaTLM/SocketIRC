#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "lib/cmd.h"

#define MAX_INPUT_LEN 100

char *read_line() {
    char *input = malloc(MAX_INPUT_LEN * 8);
    char c = (char) getchar();
    int i = 0;
    while (c != '\n' && i < MAX_INPUT_LEN) {
        *(input + i) = c;
        c = (char) getchar();
        i++;
    }
    return input;
}

int main() {
    struct CommandData cd;
    printf("Command>");
    if (cd_parse(&cd, read_line()) != 0) {
        printf("Invalid Command!\n");
    } else {
        cd_show(&cd);
    }
    return 0;
}