#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define UNIX_MAX_PATH 108

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
    
}