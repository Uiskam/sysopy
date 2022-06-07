#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/un.h>

#define MAX_CLIENT_NUMBER 16

int init_local_scket(char* path) {
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
    return socket_fd;
}

int init_remote_socket(int port_nb) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd == -1) {
        perror("remote socket creation error");
        exit(1);
    }
    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    remote_addr.sin_port = htons(port_nb);
    if(bind(socket_fd, (struct sockaddr *) &remote_addr, sizeof(remote_addr))) {
        perror("local socket bind error");
        exit(1);
    }
    if(listen(socket_fd, MAX_CLIENT_NUMBER)) {
        perror("remote socket listen error");
        exit(1);
    }
    return socket_fd;
}

int main(int argc, char** argv) {
    if(argc != 3) {
        puts("Wrong number of server arguments");   
        return 0;
    }
    int port_nb = atoi(argv[0]);
    if(1 < port_nb && port_nb < 1024) {
        puts("restircet port nb was given");
        return 0;
    }
    int local_socket = init_local_scket(argv[2]);
    int remote_sccket = init_remote_socket(port_nb);
}