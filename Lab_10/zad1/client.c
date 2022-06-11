#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/un.h>
#include <signal.h>
#include "common.h"

#define UNIX_MAX_PATH 108
char board[3][3];
int server_socket;
char my_symbol;
char opponent_symbol;

void init_board() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++)
            board[i][j] = ' ';
    }
}

void display() {
    for(int i = 0; i < 3; i++) {
        printf("%c|%c|%c\n",board[i][0], board[i][1], board[i][2]);
    }
    
}

void connect_local(char* server_scoket_path) {
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(server_socket == -1) {
        perror("Local srv socket creation error");
        exit(1);
    }
    struct sockaddr_un srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sun_family = AF_UNIX;
    strcpy(srv_addr.sun_path, server_scoket_path);
    if(connect(server_socket,(struct sockaddr *) &srv_addr, sizeof(srv_addr))) {
        perror("Local srv connecting error");
        exit(1);
    }
}

void connect_remote(char* ip_addres, int port_nb) {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1) {
        perror("Remote srv socket creation error");
        exit(1);
    }
    struct sockaddr_in srv_addr; 
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port_nb);
    srv_addr.sin_addr.s_addr = inet_addr(ip_addres);
    if (connect(server_socket, (struct sockaddr *) &srv_addr, sizeof(srv_addr))) {
        perror("Remote srv connecting error ");
        exit(1);
    }
}

int move() {
    while (1){
        int x,y;
        printf("Please give a move to play (row): ");
        scanf("%d", &x);
        printf("Please give a move to play (column): ");
        scanf("%d", &y);
        if(board[x][y] == ' ' && x >= 0 && x <= 2 && y >= 0 && y <= 2) {
            board[x][y] = my_symbol;
            return x * 3 + y;
        }
        puts("This filed is already taken or coordinates are not valid!");
    }
}

int check_game_for_completion() { //returns 1 if I won, 2 if opponent won, 0 for draw, 10 if game is not ended
    for(int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            if(board[i][0] == my_symbol)
                return 1;
            return -1;
        }
    }
    for(int j = 0; j < 3; j++) {
        if (board[0][j] == board[1][j] && board[1][j] == board[2][j]) {
            if(board[0][j] == my_symbol)
                return 1;
            return -1;
        }
    }
    if(board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        if(board[0][0] == my_symbol)
            return 1;
        return -1; 
    }
    if(board[2][0] == board[1][1] && board[1][1] == board[0][2]) {
        if(board[2][0] == my_symbol)
            return 1;
        return -1; 
    }
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(board[i][j] != ' ')
                return 10;
        }
    }
    return 0;
}

void listen_to_server(char* my_name) {
    /*
    rzeczy na które musi umiec odpowiedziec klient:
        nazwa zajęta JEST
        ping JEST
        ruch przeciwnika JEST
        wypisanie z listy JEST
        rozpoczęcie gry JEST
    rzeczy które wysyła klient:
        swój ruch JEST
        koniec gry JEST
        ping JEST
        swoja nazwa JEST
    */ 
    struct pollfd *server = malloc(sizeof(struct pollfd));
    server->fd = server_socket;
    server->events = POLLIN;
    char msg_from_srv[MAX_MSG_LENGTH + 1], msg_to_srv[MAX_MSG_LENGTH + 1];
    sprintf(msg_to_srv, "%s", my_name);
    int msg_to_srv_length = sizeof(my_name), bytes_read;
    if(write(server->fd, msg_to_srv, msg_to_srv_length) == -1) {
                perror("Name msg sending error");
    }
    msg_to_srv[0] = '\0';

    while (1) {
        poll(server, 1, -1);
        if ((bytes_read = read(server->fd, msg_from_srv, MAX_MSG_LENGTH)) == -1) {
            free(server);
            perror("Server reading error ");
            exit(1);
        }  
        msg_from_srv[bytes_read] = '\0';
        
        if(strcmp(msg_from_srv, "kick") == 0) { //reakcja na bycie wyrzucnowym z serwera
            puts("I have been kicked out");
            free(server);
            return;
        }  

        if(msg_to_srv[0] != '\0') {
            if(write(server->fd, msg_to_srv, msg_to_srv_length) == -1) {
                perror("Msg sending error");
            }
        }

        if(strcmp(msg_from_srv, "name taken") == 0) { //nazwa zajęta
            puts("My name is already taken");
            free(server);
            return;

        } else if(strcmp(msg_from_srv, "ping") == 0) { //ping
            sprintf(msg_to_srv, "ping");
            msg_to_srv_length = sizeof("ping");

        } else if(atoi(msg_from_srv) >= 0 && atoi(msg_from_srv) <= 9) { //odebranie i wysłanie ruchu (ew koniec gry jeżeli nie ma miejsca na planszy lub ktoś wygrał)
            int opponent_move = atoi(msg_from_srv);
            int x = opponent_move / 3, y = opponent_move % 3;
            if(x < 0 || x > 2 || y < 0 || y > 2 || board[x][y] != ' ') {
                printf("Recived incorrect move %d %d\n", x, y);
                sprintf(msg_to_srv, "wrong move given");
                msg_to_srv_length = sizeof("wrong move given");
            } else {
                board[x][y] = opponent_symbol;
                int game_result = check_game_for_completion();
                if(game_result > 1) {
                    int move_to_play = move();
                    sprintf(msg_to_srv, "%d", move_to_play);
                    msg_to_srv_length = sizeof('1');    
                } else {
                    sprintf(msg_to_srv, "end");
                    msg_to_srv_length = sizeof("end");
                    if(game_result == 1) {
                        puts("I won!!!");
                    } else if (game_result == 0) {
                        puts("draw");
                    } else {
                        puts("I lost :(");
                    }
                    free (server);
                    return;
                } 
                
            }          
        } else if(atoi(msg_from_srv) == X_SYMBOL || atoi(msg_from_srv) == Y_SYMBOL) { //rozpoczecie gry
            if(atoi(msg_from_srv) == X_SYMBOL) {
                my_symbol = 'x';
                opponent_symbol = 'o';
                int move_to_play = move();
                sprintf(msg_to_srv, "%d", move_to_play);
                msg_to_srv_length = sizeof('1');    
            } else {
                my_symbol = 'o';
                opponent_symbol = 'x';
                msg_to_srv[0] = '\0';
            }
        } else if(strcmp(msg_from_srv, "no space") == 0) {
            puts("Server is full!");
            return;
        }
        else {
            printf("Unknown msg from server: %s\n", msg_from_srv);
        }
    }
    
    
}

int main(int argc, char ** argv) {
    
    if(argc != 4 && argc != 5 && ((argc != 4 || strcmp("local", argv[2]) == 0) || (argc != 5 || strcmp("remote", argv[2]) == 0))) {
        puts("Wrong number of args!");
        return 0;
    }
    if(strlen(argv[1]) > CLIENT_MAX_NAME_SIZE) {
        puts("Name is too long");
        return 0;
    }
    if(strcmp("local", argv[2]) == 0) 
        connect_local(argv[2]);
    else if (strcmp("remote", argv[2]) == 0) {
        connect_remote(argv[3], atoi(argv[4]));
    }
    return 0;
}