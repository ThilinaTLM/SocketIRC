#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>


#include "lib/cmd.h"
#include "lib/cprofile.h"
#include "lib/cpool.h"
#include "lib/console.h"

#define IP 0
#define PORT 8080

// Global Variables
struct ClientPool *CPOOL;
int SERVER_SOCK_FD;
int WAIT_TIME;
int MAX_CLIENTS;
int MAX_CLIENTS_EXTRA;
pthread_mutex_t ACCEPT_LOCK;


// Server
void configure_server(struct sockaddr_in *addr);

int start_threads();

int accept_client_socket(struct sockaddr_in *client_addr); // thread safe

// Client Specific
int client_connection(struct ClientData *cp);

void client_command_handler(struct ClientData *cp);

int read_client_command(struct ClientData *cp, struct CommandData *cmd);

void clean_client_connection(struct ClientData *cp);

// Client Command Handlers
void wait_for_join(struct ClientData *cp);

void client_cmd_nick(struct ClientData *cp, const char *nickname);

void client_cmd_quit(struct ClientData *cp);

void client_cmd_who(struct ClientData *cp);

void client_cmd_whosis(struct ClientData *cp, const char *nickname);

void client_cmd_privmsg(struct ClientData *cp, const char *dest_name, const char *msg);

void client_cmd_bcastmsg(struct ClientData *cp, const char *msg);

void client_cmd_time(struct ClientData *cp);


int main(int argc, char const **argv) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // localhost
    addr.sin_port = htons(PORT);

    printf("[INFO]: Welcome!\n");


    MAX_CLIENTS = 10;
    MAX_CLIENTS_EXTRA = 3;
    WAIT_TIME = 5;

    CPOOL = cpool_create(MAX_CLIENTS + MAX_CLIENTS_EXTRA);

    pthread_mutex_lock(&ACCEPT_LOCK);
    start_threads();
    configure_server(&addr);
    pthread_mutex_unlock(&ACCEPT_LOCK);

    while (1) {
        sleep(1);
    }
    return 0;
}

void configure_server(struct sockaddr_in *addr) {
    if ((SERVER_SOCK_FD = socket(AF_INET, SOCK_STREAM, IP)) == 0) {
        perror("[ERROR]: Socket Failed!");
        exit(EXIT_FAILURE);
    }

    // Attach socket such a way that port can be reused
    int on = 1;
    if (setsockopt(SERVER_SOCK_FD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(on))) {
        perror("[ERROR]: Address already in use.");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    printf("[INFO]: Listening at PORT 8080\n");
    if (bind(SERVER_SOCK_FD, (struct sockaddr *) addr, sizeof(*addr)) < 0) {
        printf("[ERROR]: Socket binding failed");
        exit(EXIT_FAILURE);
    }
}

int accept_client_socket(struct sockaddr_in *client_addr) {
    int client_sock_fd;
    int client_addr_len = sizeof(*client_addr);

    pthread_mutex_lock(&ACCEPT_LOCK);
    if (listen(SERVER_SOCK_FD, 3) < 0) {
        printf("[ERROR]: Cannot listening to the socket\n");
        exit(EXIT_FAILURE);
    }
    client_sock_fd = accept(SERVER_SOCK_FD, (struct sockaddr *) client_addr, (socklen_t *) &client_addr_len);
    if (client_sock_fd < 0) {
        printf("[ERROR]: Error while accepting client socket\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_unlock(&ACCEPT_LOCK);
    return client_sock_fd;
}

int start_threads() {
    struct ClientData *cp;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        cp = cp_create();
        cpool_add(CPOOL, cp);
        pthread_create(&(cp->thread), NULL, (void *(*)(void *)) client_connection, cp);
    }
    printf("[INFO]: %02d Connections are listening\n", MAX_CLIENTS);
    return 0;
}

int client_connection(struct ClientData *cp) {
    struct sockaddr_in client_addr;
    char ip_address[16];

    // make thread alive
    cp->status = STATUS_CONNECTED;

    // accept client socket
    cp->socket_fd = accept_client_socket(&client_addr);

    // set ip address
    inet_ntop(AF_INET, &client_addr.sin_addr, ip_address, INET_ADDRSTRLEN);
    cp_set_ip_address(cp, ip_address);

    printf("[CLIENT]: Client Connection Accepted! (SOCK_FD: %d)\n", cp->socket_fd);

    client_command_handler(cp);
    return 0;
}

int read_client_command(struct ClientData *cp, struct CommandData *cmd) {
    int status = read(cp->socket_fd, cmd, sizeof(*cmd));
    if (status == 0) {
        clean_client_connection(cp);
    }
    return status;
}

void client_command_handler(struct ClientData *cp) {
    struct CommandData cmd;
    wait_for_join(cp);
    while (1) {
        read_client_command(cp, &cmd);
        switch (cmd.code) {
            case C_QUIT:
                client_cmd_bcastmsg(cp, cmd.arg1);
                client_cmd_quit(cp);
                pthread_exit(0);
            case C_NICK:
                client_cmd_nick(cp, cmd.arg1);
                break;
            case C_WHO:
                client_cmd_who(cp);
                break;
            case C_WHOSIS:
                client_cmd_whosis(cp, cmd.arg1);
                break;
            case C_TIME:
                client_cmd_time(cp);
                break;
            case C_PRIVMSG:
                client_cmd_privmsg(cp, cmd.arg1, cmd.arg2);
                break;
            case C_BCASTMSG:
                client_cmd_bcastmsg(cp, cmd.arg1);
                break;
        }
    }
}

void clean_client_connection(struct ClientData *cp) {
    char msg_buffer[MSG_LENGTH];
    struct ClientData *cp_itter;

    if (cp->status == STATUS_JOINED) {
        sprintf(msg_buffer, "[SERVER]: %s is disconnected.", cp->nickname);
        for (int i = 0; i < CPOOL->size; i++) {
            cp_itter = cpool_get(CPOOL, i);
            if (cp_itter == cp && cp_itter->status == STATUS_JOINED) {
                cp_send_message(cp_itter, msg_buffer);
            }
        }
    }

    cp_clean(cp);
    printf("[CLIENT]: Client Disconneted (SOCK_FD: %d)\n", cp->socket_fd);
    pthread_exit(0);
}

/***
 *       ___  _     ___  ___  _  _  _____    ___  ___   __  __  __  __    _    _  _  ___   ___
 *      / __|| |   |_ _|| __|| \| ||_   _|  / __|/ _ \ |  \/  ||  \/  |  /_\  | \| ||   \ / __|
 *     | (__ | |__  | | | _| | .` |  | |   | (__| (_) || |\/| || |\/| | / _ \ | .` || |) |\__ \
 *      \___||____||___||___||_|\_|  |_|    \___|\___/ |_|  |_||_|  |_|/_/ \_\|_|\_||___/ |___/
 *
 */

void wait_for_join(struct ClientData *cp) {
    struct CommandData cmd;
    char msg_buffer[MSG_LENGTH];

    read_client_command(cp, &cmd);
    while (cmd.code != C_JOIN) {
        sprintf(msg_buffer, "[WARN][SERVER]: Use JOIN command to join as user!");
        cp_send_message(cp, &msg_buffer[0]);
        read_client_command(cp, &cmd);
    }
    cp_set_name(cp, (const char *) &cmd.arg2, (const char *) &cmd.arg1);
    cp_set_joined_tme(cp);
    cp->status = STATUS_JOINED;

    sprintf(msg_buffer, "[SERVER]: Joined! | USERNAME: '%s', NAME: '%s', IP: %s",
            cp->username, cp->realname, cp->ip_address);
    cp_send_message(cp, msg_buffer);
}

void client_cmd_bcastmsg(struct ClientData *cp, const char *msg) {
    char msg_buffer[MSG_LENGTH];
    sprintf(msg_buffer, "[BCAST-MSG][%s]: %s", cp->nickname, msg);
    for (int i = 0; i < CPOOL->size; i++) {
        struct ClientData *cp_itter = cpool_get(CPOOL, i);
        if (cp_itter == cp) continue;
        cp_send_message(cp_itter, msg_buffer);
    }
}

void client_cmd_quit(struct ClientData *cp) {
    char msg_buffer[MSG_LENGTH];
    sprintf(msg_buffer, "[SERVER]: Connection time %ld secs, Bye %s", cp_get_duration(cp), cp->nickname);
    cp_send_message(cp, msg_buffer);
    clean_client_connection(cp);
}

void client_cmd_nick(struct ClientData *cp, const char *nickname) {
    char msg_buffer[MSG_LENGTH];
    cp_set_nickname(cp, nickname);
    sprintf(msg_buffer, "[SERVER]: Nickname changed to '%s'", nickname);
    cp_send_message(cp, msg_buffer);
}

void client_cmd_privmsg(struct ClientData *cp, const char *dest_name, const char *msg) {
    char msg_buffer[MSG_LENGTH];
    struct ClientData *dest = cpool_find_byNickname(CPOOL, dest_name);
    if (dest != NULL) {
        sprintf(msg_buffer, "[MSG][%s]: %s", cp_get_nick_or_username(cp), msg);
        cp_send_message(dest, msg_buffer);
        return;
    }
    dest = cpool_find_byUsername(CPOOL, dest_name);
    if (dest != NULL) {
        sprintf(msg_buffer, "[MSG][%s]: %s", cp_get_nick_or_username(cp), msg);
        cp_send_message(dest, msg_buffer);
        return;
    }
    sprintf(msg_buffer, "[SERVER]: Couldn't find, %s", dest_name);
    cp_send_message(cp, msg_buffer);
}

void client_cmd_who(struct ClientData *cp) {
    char msg_buffer[MSG_LENGTH];
    sprintf(msg_buffer, "[SERVER]: Online users:");
    cp_send_message(cp, msg_buffer);

    struct ClientData *cp_itter;
    int joiner_index = 1;
    for (int i = 0; i < CPOOL->size; i++) {
        cp_itter = cpool_get(CPOOL, i);
        if (cp_itter->status == STATUS_JOINED) {
            char *details = cp_to_string(cp_itter);
            sprintf(msg_buffer, "\t%02d - %s |", joiner_index++, details);
            free(details);
            cp_send_message(cp, msg_buffer);
        }
    }
}

void client_cmd_whosis(struct ClientData *cp, const char *nickname) {
    char msg_buffer[MSG_LENGTH];
    struct ClientData *dest = cpool_find_byNickname(CPOOL, nickname);
    if (dest != NULL) {
        char *details = cp_to_string(dest);
        sprintf(msg_buffer, "[SERVER]: %s", details);
        cp_send_message(dest, msg_buffer);
        return;
    }

    sprintf(msg_buffer, "[SERVER]: Couldn't find, %s", nickname);
    cp_send_message(cp, msg_buffer);
}

void client_cmd_time(struct ClientData *cp) {
    time_t timer;
    char msg_buffer[MSG_LENGTH];
    struct tm *tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(msg_buffer, MSG_LENGTH, "[SERVER][TIME]: %Y-%m-%d %H:%M:%S", tm_info);
    cp_send_message(cp, msg_buffer);
}