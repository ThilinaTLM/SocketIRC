#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#include "lib/cmd.h"
#include "lib/cprofile.h"

#define IP 0
#define PORT 8080


void client_connection(struct ClientProfile *cp) {
    char buffer[1024];
    while (read(cp->socket_fd, buffer, 1024)) {
        printf("[Client%d]: %s\n", cp->client_id, buffer);
        memset(buffer, 0, 1024 * sizeof(buffer[0]));
    }
}

void configure_server(int *socket_fd, struct sockaddr_in *addr) {
    *socket_fd = socket(AF_INET, SOCK_STREAM, IP);

    // Attach socket such a way that port can be reused
    int on = 1;
    if (setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(on)) < 0) {
        printf("Address already in use\n");
        exit(EXIT_FAILURE);
    }

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY; // localhost
    addr->sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    printf("Listening at 8080\n");
    if (bind(*socket_fd, (struct sockaddr *) addr, sizeof(*addr))) {
        printf("Socket binding failed");
    }
}

int main(int argc, char const *argv[]) {
    int server_fd;
    struct sockaddr_in addr;

    // Configure Server Socket
    configure_server(&server_fd, &addr)

    struct ClientProfile client_profiles[10];
    while (1) {
        listen(server_fd, 3);
        int client_fd = accept(server_fd, (struct sockaddr *) &address, (socklen_t *) &addrlen);
        if (client_fd < 0) break;

        printf("Connection established. [SFD: %d]\n", client_fd);
        client_profiles[client_count].client_id = client_count + 1;
        client_profiles[client_count].socket_fd = client_fd;
        pthread_t thread;
        pthread_create(&thread, NULL, (void *(*)(void *)) client, &client_profiles[client_count]);
        client_count++;
    }
}

