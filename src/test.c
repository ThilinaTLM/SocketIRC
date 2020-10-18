#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

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

void runner() {
    printf("Hello World!\n");
    sleep(2);
    printf("END THREAD\n");
    pthread_exit(0);
}

int main() {
    pthread_t thread;
    printf("Start Thread 01\n");
    pthread_create(&thread, NULL, (void *) runner, NULL);
    pthread_join(thread, NULL);
    sleep(2);
    printf("Start Thread 02\n");
    pthread_create(&thread, NULL, (void *) runner, NULL);
    pthread_join(thread, NULL);
    sleep(2);
}