#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "lib/cmd.h"
#include "lib/console.h"
#include "lib/cprofile.h"

#define HOST "127.0.0.1"
#define PORT 8080

void receiver(const int *sock_fd);
int create_socket();

int main(int argc, char const **argv) {
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t thread_receiver;

    const char* ip_address = HOST;
    const int port = PORT;

    // Create Socket
    sock = create_socket();

    // Setup Server Adress
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
        printf("[ERROR]: Invalid host name or port\n");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("[ERROR]: Cannot connect to the server\n");
        exit(EXIT_FAILURE);
    }

    // Start Receiver Thread
    pthread_create(&thread_receiver, NULL, (void *(*)(void *)) receiver, &sock);

    char input[MAX_INPUT_LEN];
    char ps1[50] = "client";
    struct CommandData cd;
    while (1) {
        printf("%s>", ps1);
        read_line(input);
        if (cd_parse(&cd, input) != 0) {
            console_show_invalid(50);
            continue;
        }

        console_show_sent(50);

        if (cd.code == C_JOIN || cd.code == C_NICK) {
            strcpy(ps1, cd.arg1);
        } else if (cd.code == C_QUIT) {
            send(sock, &cd, sizeof(struct CommandData), 0);
            sleep(5);
            exit(0);
        }
        if (send(sock, &cd, sizeof(struct CommandData), 0) <= 0) {
            console_clear_current_line();
            printf("[ERROR]: Connection lost!\n");
        }
    }
}

int create_socket() {
    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("[ERROR]: Socket creation failed.\n");
        exit(EXIT_FAILURE);
    }
    return sock;
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