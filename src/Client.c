#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "lib/cmd.h"
#include "lib/console.h"
#include "lib/cprofile.h"

#define TICK_MSG_INDENT 50

// Global Variables ----------------------------------------------------------------------------------------------------
int PORT;
const char* HOST;
int CONN_SOCKET;
// ---------------------------------------------------------------------------------------------------------------------

void configure_socket();
void parse_arguments(int argc, char const **argv);
void receiver(const int *sock_fd);

int main(int argc, char const **argv) {
    pthread_t thread_receiver;

    parse_arguments(argc, argv);

    // Config Client Socket
    configure_socket();

    // Start Receiver Thread
    pthread_create(&thread_receiver, NULL, (void *(*)(void *)) receiver, &CONN_SOCKET);

    char input[MAX_INPUT_LEN];
    char ps1[50] = "client";
    struct CommandData cd;

    while (1) {
        printf("%s>", ps1);
        read_line(input);
        if (cd_parse(&cd, input) != 0) {
            console_show_invalid(TICK_MSG_INDENT);
            continue;
        }

        console_show_sent(TICK_MSG_INDENT);

        if (cd.code == C_JOIN || cd.code == C_NICK) {
            strcpy(ps1, cd.arg1);
        } else if (cd.code == C_QUIT) {
            send(CONN_SOCKET, &cd, sizeof(struct CommandData), 0);
            pthread_join(thread_receiver, 0);
            exit(0);
        }
        if (send(CONN_SOCKET, &cd, sizeof(struct CommandData), 0) <= 0) {
            console_clear_current_line();
            printf("[ERROR]: Connection Lost!\n");
        }
    }
}

void parse_arguments(int argc, char const **argv) {
    if (argc < 3) {
        printf("[ERROR]: Invalid commandline arguments\n");
    }

    int arg2;
    arg2 = (int) strtol(*(argv + 2), 0, 10);

    if (arg2 <= 0) {
        printf("[ERROR]: Invalid commandline arguments\n");
        exit(EXIT_FAILURE);
    }

    HOST = *(argv + 1);
    PORT = arg2;

    printf("[INFO]: Client Configuration:\n");
    printf("\tHOST ADDRESS: %s\n", HOST);
    printf("\tPORT: %d\n", PORT);
}

void configure_socket() {
    struct sockaddr_in serv_addr;

    // Initialize Socket
    if ((CONN_SOCKET = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("[ERROR]: Socket creation failed.\n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, HOST, &serv_addr.sin_addr) <= 0) {
        printf("[ERROR]: Invalid host name or port\n");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(CONN_SOCKET, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("[ERROR]: Couldn't connect to the server\n");
        exit(EXIT_FAILURE);
    }
}

void receiver(const int *sock_fd) {
    char data[1024];
    while (read(*sock_fd, data, MSG_LENGTH) > 0) {
        console_clear_current_line();
        printf("%s\n", data);
    }
    console_clear_current_line();
    printf("[ERROR]: Connection lost!\n");
    exit(EXIT_FAILURE);
}