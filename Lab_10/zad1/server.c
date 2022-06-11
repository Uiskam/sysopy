#include <sys/socket.h>
#include <sys/types.h>
#include <poll.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/un.h>
#include <pthread.h>
#include "common.h"



int local_socket, remote_socket, cur_client_number;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

struct ClientInfo {
    char name[CLIENT_MAX_NAME_SIZE];
    int client_socket;
    int responds; //odpowiada i jakiś klient jest przypisany
    int opponent_socket;
};

struct ClientInfo clients[MAX_CLIENT_NUMBER];

/*
    zadania serwera:
        inicjalizacja gniazd
        cykliczne pingowanie
        łączenie w pary klientów
        obłsuga sygnałów od klienta:
            swój ruch 
            koniec gry 
            ping
            swoja nazwa

*/

void init_local_socket(char* path) {
    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(socket_fd == -1) {
        perror("local socket creation error");
        exit(1);
    }
    struct sockaddr_un local_addr;
    memset(&local_addr, 0 , sizeof(local_addr));
    local_addr.sun_family = AF_UNIX;
    strcpy(local_addr.sun_path, path);
    unlink(path);
    if(bind(socket_fd, (struct sockaddr *) &local_addr, sizeof(local_addr)) == -1) {
        perror("local socket bind error");
        exit(1);
    }
    if(listen(socket_fd, MAX_CLIENT_NUMBER)) {
        perror("local socket listen error");
        exit(1);
    }
    local_socket = socket_fd;
}

void init_remote_socket(int port_nb) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1) {
        perror("remote socket creation error");
        exit(1);
    }
    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRES);
    remote_addr.sin_port = htons(port_nb);
    if(bind(socket_fd, (struct sockaddr *) &remote_addr, sizeof(remote_addr))) {
        perror("local socket bind error");
        exit(1);
    }
    if(listen(socket_fd, MAX_CLIENT_NUMBER)) {
        perror("remote socket listen error");
        exit(1);
    }
    remote_socket = socket_fd;
}

void* ping() {
    while (1) {
        sleep(300);
        puts("sending pings");
        pthread_mutex_lock(&clients_mutex);
        for(int i = 0; i < MAX_CLIENT_NUMBER; i++) {
            if(clients[i].client_socket >= 0 && clients[i].responds == 0) {
                if(write(clients[i].client_socket, "kick", sizeof("kick") == -1)) {
                    printf("kick sending to %s error", clients[i].name);
                    perror("");
                }
                clients[i].client_socket = -1;
                sprintf(clients[i].name, " ");
                clients[i].opponent_socket = -1;
                cur_client_number--;
            }
        }
        for(int i = 0; i < MAX_CLIENT_NUMBER; i++) {
            if(clients[i].responds == 1) {
                write(clients[i].client_socket, "ping", sizeof("ping"));
                clients[i].responds = 0;
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
}

int find_socket_with_msg() {
    struct pollfd *socktes_to_listen = malloc((cur_client_number + 2) * sizeof(struct pollfd));
    socktes_to_listen[0].fd = local_socket;
    socktes_to_listen[0].fd = remote_socket;
    socktes_to_listen[0].events = POLLIN;
    socktes_to_listen[1].events = POLLIN;
    pthread_mutex_lock(&clients_mutex);
    for(int i = 2; i < cur_client_number + 2; i++) {
        socktes_to_listen[i].fd = clients[i - 2].client_socket;
        socktes_to_listen[i].events = POLLIN;
    }
    pthread_mutex_unlock(&clients_mutex);
    poll(socktes_to_listen, cur_client_number + 2, -1);

    for(int i = 0; i < cur_client_number + 2; i++) {
        if(socktes_to_listen[i].revents == POLLIN) {
            int socket_with_msg = socktes_to_listen[i].fd;
            if (socket_with_msg == local_socket || socket_with_msg == remote_socket) {
                free(socktes_to_listen);
                return accept(socket_with_msg, NULL, NULL);
            }
            free(socktes_to_listen);
            return socket_with_msg;
        }
    }
    free(socktes_to_listen);
    return -1;
}

int name_taken(char* name) { //zwraca 1 jezli nazwa jest zajeta 0 wpp
    for(int i = 0; i < MAX_CLIENT_NUMBER; i++) {
        if(clients[i].client_socket != -1) {
            if(strcmp(clients[i].name, name) == 0)
                return 1;
        }
    }
    return 0;
}

int find_free_space() {
    for(int i = 0; i < MAX_CLIENT_NUMBER; i++) {
        if(clients[i].client_socket == -1) {
            return i;
        }
    }
    return -1;
}

int find_opponent(int me) {
    for(int i = 0; i < MAX_CLIENT_NUMBER; i++) {
        if(i == me) continue;
        if(clients[i].client_socket != -1 && clients[i].responds == 1 && clients[i].opponent_socket == -1) {
            return i;
        }
    }
    return -1;
}

int find_client_with_socket(int socket_fd) {
    for(int i = 0; i < MAX_CLIENT_NUMBER; i++) {
        if(clients[i].client_socket == socket_fd)
            return i;
    }
    return -1;
}

void* server_start() {
    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping, NULL);

    while (1) {
        int socket = find_socket_with_msg(), bites_read;
        pthread_mutex_lock(&clients_mutex);
        int cur_client_index = find_client_with_socket(socket);
        if(cur_client_index == -1) {
            perror("critcal error client not found");
            for(int i = 0; i < MAX_CLIENT_NUMBER; i++) {
                if(clients[i].client_socket != -1) {
                    write(clients[i].client_socket, "kick", sizeof("kick"));
                }
            }
            exit(1);
        }
        char msg_recived[MAX_MSG_LENGTH];
        if((bites_read = read(socket, msg_recived, MAX_MSG_LENGTH)) == -1) {
            perror("msg reading error");
            continue;
        }
        msg_recived[bites_read] = '\0';
        if(strcmp(msg_recived, "ping") == 0){
            printf("recived ping from %s\n", clients[cur_client_index].name);
            clients[cur_client_index].responds = 1;
        } else if(strcmp(msg_recived, "wrong move given")) {
            printf("Critacl error in game of %s\n", clients[cur_client_index].name);
            if(write(clients[cur_client_index].client_socket, "kick", sizeof("kick")) == -1 || write(clients[cur_client_index].opponent_socket, "kick", sizeof("kick"))) {
                perror("game break sending error");
                continue;
            }
            exit(1);
        } else if(atoi(msg_recived) >= 1 && atoi(msg_recived) <= 9) {
            if(write(clients[cur_client_index].opponent_socket, msg_recived, sizeof(msg_recived)) == -1) {
                printf("Move sending to oppoent of %s error", clients[cur_client_index].name);
                perror("");
            }
        } else if(strcmp(msg_recived, "end") == 0) {
            if(write(clients[cur_client_index].client_socket, "kick", sizeof("kick")) == -1 || write(clients[cur_client_index].opponent_socket, "kick", sizeof("kick"))) {
                printf("game ending sending of %s error", clients[cur_client_index].name);
                perror("");
                continue;
            }
            int op_index = find_client_with_socket(clients[cur_client_index].opponent_socket);
            if(op_index == -1) {
                perror("critacl error no2");
                exit(1);
            }
            clients[cur_client_index].client_socket = -1;
            clients[cur_client_index].opponent_socket = -1;
            clients[cur_client_index].responds = 0;
            strcpy(clients[cur_client_index].name, " ");
            clients[op_index].client_socket = -1;
            clients[op_index].opponent_socket = -1;
            clients[op_index].responds = 0;
            strcpy(clients[op_index].name, " ");
        } else {
            printf("client %s tries to log in\n", msg_recived);
            if(name_taken(msg_recived)) {
                printf("Name %s is taken already\n", msg_recived);
                if(write(socket, "name taken", sizeof("name taken")) == -1) {
                    printf("Msg sending error to %s", msg_recived);
                    perror("");
                }
            } else {
                int new_client = find_free_space();
                if(new_client != -1) {// dodawanie nowego 
                    clients[new_client].client_socket = socket;
                    strcpy(clients[new_client].name, msg_recived);
                    clients[new_client].responds = 1;
                    clients[new_client].opponent_socket = -1;
                    int oppoenent = find_opponent(socket);
                    if(oppoenent != -1) {
                        clients[new_client].opponent_socket = clients[oppoenent].client_socket;
                        clients[oppoenent].opponent_socket = clients[new_client].client_socket;
                        if(rand() % 2) {
                            if(write(clients[new_client].client_socket, "10", sizeof("10") == -1)) {
                                printf("Game init %s error",clients[new_client].name);
                                perror("");
                            }
                            if(write(clients[oppoenent].client_socket, "11", sizeof("11") == -1)) {
                                printf("Game init %s error",clients[oppoenent].name);
                                perror("");
                            }
                        } else {
                            if(write(clients[oppoenent].client_socket, "10", sizeof("10") == -1)) {
                                printf("Game init %s error",clients[oppoenent].name);
                                perror("");
                            }
                            if(write(clients[new_client].client_socket, "11", sizeof("11") == -1)) {
                                printf("Game init %s error",clients[new_client].name);
                                perror("");
                            }
                        }
                    }
                } else {
                    printf("Server full. No space for %s\n", msg_recived);
                    if(write(socket, "no space", sizeof("no space")) == -1) {
                        printf("No space sendin error to %s", msg_recived);
                        perror("");
                    }
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }
        
}

int main(int argc, char** argv) {
    cur_client_number = 0;
    if(argc != 3) {
        puts("Wrong number of server arguments");   
        return 0;
    }
    int port_nb = atoi(argv[0]);
    if(1 < port_nb && port_nb < 1024) {
        puts("restircet port nb was given");
        return 0;
    }
    init_local_socket(argv[2]);
    init_remote_socket(port_nb);
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, server_start, NULL);
    pthread_join(server_thread, NULL);
}