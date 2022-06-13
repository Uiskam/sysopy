#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>

#include "shared.h"

struct Client {
    char name[32];
    int socket;
    int is_alive;
    int opponentSocket; //-1 -> no opponent
};


struct Client clients[MAX_CLIENTS];
int clients_no = 0;
int local_socket;
int network_socket;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void local_socket_init(char *path) {
    if ((local_socket = socket(AF_UNIX, SOCK_STREAM, 0)) <= 0) {
        perror("socket failed");
        exit(1);
    }

    struct sockaddr_un sa;
    sa.sun_family = AF_UNIX;
    strcpy(sa.sun_path, path);
    unlink(path);
    if (bind(local_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        perror("local bind failed");
        exit(1);
    }
    if (listen(local_socket, MAX_CLIENTS) < 0) {
        perror("local listen failed");
        exit(1);
    }
}

void network_socket_init(char *path) {
    int port = atoi(path);
    if(port > 65535 || port < 1024) {
        puts("wrong port nb, port must be greater than 1023 and lesser then 65536");
        exit(1);
    }
    if ((network_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
        perror("remote socket failed");
        exit(1);
    }

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons((unsigned short)port);
    sa.sin_addr.s_addr = INADDR_ANY;

    if (bind(network_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
        perror("remote bind failed");
        exit(1);
    }
    if (listen(network_socket, MAX_CLIENTS) < 0) {
        perror("remote listen failed");
        exit(1);
    }
}

int monitoring_clients() {
    struct pollfd *sockets = malloc((clients_no + 2) * sizeof(struct pollfd));
    sockets[0].fd = network_socket;
    sockets[1].fd = local_socket;
    sockets[0].events = POLLIN;
    sockets[1].events = POLLIN;

    pthread_mutex_lock(&mutex);
    for (int i = 2; i < clients_no + 2; i++) {
        sockets[i].fd = clients[i - 2].socket;
        sockets[i].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);
    poll(sockets, 2 + clients_no, -1); //wait inf for clients

    for (int i = 0; i < clients_no + 2; i++) {
        if (sockets[i].revents == POLLIN) {
            int socket = sockets[i].fd;
            if (socket == local_socket || socket == network_socket) {
                return accept(socket, NULL, NULL);
            }
            return socket;
        }
    }

    free(sockets);
    return -1;
}


void remove_dead_clients() {
    for (int i = 0; i < clients_no; i++) {
        if (!clients[i].is_alive) {
            if (i < clients_no - 1) {
                clients[i] = clients[clients_no - 1];
            }
            clients_no--;
        }
    }
}

void *ping_clients() {
    while (1) {
        sleep(30);
        pthread_mutex_lock(&mutex);
        //printf("ping\n");
        remove_dead_clients();
        for (int i = 0; i < clients_no; i++) {
            if (!clients[i].is_alive) continue;
            if(send(clients[i].socket, "P", sizeof("P"), 0) == -1) {
                perror("ping sending error");
            }
            printf("removing client %s (%d)\n", clients[i].name, clients[i].socket);
            clients[i].is_alive = 0; //ping back set to 1
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void clients_listen() {
    pthread_t pt;
    pthread_create(&pt, NULL, ping_clients, NULL);

    while (1) {
        int socket = monitoring_clients();
        char msg[MSG_LEN];
        int bytes_read;
        if ((bytes_read = recv(socket, msg, MSG_LEN, 0)) <= 0) {
            sleep(1);
            continue;
        }
        msg[bytes_read] = '\0';
        pthread_mutex_lock(&mutex);
        if (strcmp(msg, "P") == 0) {
            for (int i = 0; i < clients_no; i++) {
                if (clients[i].socket == socket) {
                    clients[i].is_alive = 1;
                }
            }
        } else if (strlen(msg) == 1 && msg[0] >= '1' && msg[0] <= '9') {
            for (int i = 0; i < clients_no; i++) {
                if (clients[i].socket == socket) {
                    if (clients[i].opponentSocket == -1) {
                        if(send(socket, "no oponnent", sizeof("no oponnent"), 0) == -1) {
                            perror("no opponent sending error");
                        }
                    } else {
                        printf("move from %s (%d) to %d\n", clients[i].name, clients[i].socket, clients[i].opponentSocket);
                        if(send(clients[i].opponentSocket, msg, sizeof(msg), 0) == -1) {
                            perror("move sending error");
                        }
                    }
                }
            }
        } else if (strlen(msg) == 2 && (msg[0] == 'W' || msg[0] == 'L' || msg[0] == 'D')) {
            for (int i = 0; i < clients_no; i++) {
                if (clients[i].socket == socket) {
                    if (clients[i].opponentSocket == -1) {
                        clients[i].is_alive = 0;
                    } else {
                        if(send(clients[i].opponentSocket, msg, sizeof(msg), 0) == -1) {
                            perror("end msg sending error");
                        }
                        clients[i].is_alive = 0;
                        for (int j = 0; j < clients_no; j++) {
                            if (clients[j].socket == clients[i].opponentSocket) {
                                clients[j].is_alive = 0;
                                printf("game beteween %s and %s came to and end\n", clients[i].name, clients[j].name);
                                clients_no--;
                                if(i < clients_no - 1) {
                                    clients[i] = clients[clients_no - 1];
                                    if(j == clients_no -1) {
                                        j = i;
                                    }
                                }
                                clients_no--;
                                if(j < clients_no - 1) {
                                    clients[j] = clients[clients_no - 1];
                                }
                                i = 100;
                                j = 100;
                            }
                        }
                    }
                    //remove_dead_clients();
                }
            }
        } else {
            printf("%s\n", msg);
            int is_new = 1;
            for (int i = 0; i < clients_no; i++) {
                if (clients[i].socket == socket) {
                    is_new = 0;
                } else if (strcmp(msg, clients[i].name) == 0) {
                    is_new = 0;
                    printf("registered failed, name %s taken\n", msg);
                    if(send(socket, "name taken", sizeof("name taken"), 0) == -1) {
                        perror("name taken sending error");
                    }
                }
            }

            //adding new
            if (is_new == 1) {
                clients[clients_no].socket = socket;
                strcpy(clients[clients_no].name, msg);
                clients[clients_no].is_alive = 1;
                clients[clients_no].opponentSocket = -1;
                clients_no++;
                printf("registered %s\n", msg);

                //finding oponent
                int was = 0;
                for (int i = 0; i < clients_no; i++) {
                    if (clients[i].socket != socket && clients[i].opponentSocket == -1) {
                        was = 1;
                        clients[i].opponentSocket = socket;
                        clients[clients_no - 1].opponentSocket = clients[i].socket;
                        printf("paired %s (%d) with %s (%d)\n", clients[i].name, clients[i].socket, clients[clients_no -1].name, clients[clients_no -1].socket);
                        if(send(socket, "X", sizeof("X"), 0) == -1) {
                            perror("x symbol sending error");
                        }
                        if(send(clients[i].socket, "O", sizeof("O"), 0) == -1) {
                            perror("o symbol sending error");
                        }
                    }
                }
                if (!was) {
                    if(send(socket, "no oponnent", sizeof("no oponnent"), 0) == -1) {
                        perror("no opponent sending error");
                    }
                }
            }
        }

        pthread_mutex_unlock(&mutex);
    }
}

void sigint_hanlder(int signum) {
    puts("SIGINT received - closing");
    if(close(local_socket) == -1) {
        perror("local closing error");
    }
    if(close(network_socket) == -1) {
        perror("network closing error");
    }
    exit(1);
}

int main(int argc, char **argv) {
    signal(SIGINT, sigint_hanlder);
    if (argc != 3) {
        puts("wrong number of args");
        return 1;
    }
    signal(SIGPIPE, SIG_IGN);
    network_socket_init(argv[1]);
    local_socket_init(argv[2]);

    printf("server start\n");

    clients_listen();


    return 0;
}