
#ifndef SOCKETIRC_CONSOLE_H
#define SOCKETIRC_CONSOLE_H

#include <stdio.h>

#define MAX_INPUT_LEN 100

void read_line(char *input) {
    char c = (char) getchar();
    int i = 0;
    while (c != '\n' && i < MAX_INPUT_LEN - 1) {
        *(input + i) = c;
        c = (char) getchar();
        i++;
    }
    *(input + i) = 0;
}

void console_clear_current_line() {
    printf("\r");
    printf("%c[2K", 27);
}

void console_show_sent(int indent) {
    printf("\033[1A");
    printf("\033[%dC", indent);
    printf("\t--- #Sent\n");
}

void console_show_invalid(int indent) {
    printf("\033[1A");
    printf("\033[%dC", indent);
    printf("\t--- #Invalid\n");
}

void console_clear_screen() {
    printf("\033[2J\n\n");
}

void console_write_top_line(const char *msg) {
    printf("\0337");
    printf("\033[0;0");
    printf("%s", msg);
    printf("\0338");

}

#endif //SOCKETIRC_CONSOLE_H
