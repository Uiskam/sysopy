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

char *my_name;
int server_socket;
int server_socket2;
char my_figure;
char op_figure;
int my_moves[5];
int op_moves[5];
int my_moves_no = 0;
int op_moves_no = 0;

void connect_to_server(char *type, char *address) {
    if (strcmp(type, "local") == 0) {
        //connect local socket

        if ((server_socket = socket(AF_UNIX, SOCK_STREAM, 0)) <= 0) {
            perror("socket failed");
            exit(1);
        }
        struct sockaddr_un sa;
        sa.sun_family = AF_UNIX;
        strcpy(sa.sun_path, address);
        if (connect(server_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
            perror("connect local failed");
            exit(1);
        }

    } else if (strcmp(type, "remote") == 0) {
        //connect to network socket

        int port = atoi(address);
        if(port > 65535 || port < 1024) {
            puts("wrong port nb, port must be greater than 1023 and lesser then 65536");
            exit(1);
        }
        if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
            perror("socket failed");
            exit(1);
        }
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_port = htons((unsigned short) port);
        sa.sin_addr.s_addr = INADDR_ANY;
        if (connect(server_socket, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
            perror("connect network failed");
            exit(1);
        }

    } else {
        exit(1);
    }
}

void register_in_server() {
    char buff[MSG_LEN];
    sprintf(buff, "%s", my_name);
    if(send(server_socket, buff, sizeof(buff), 0) == -1) {
        perror("register msg snd error");
    }
}

void display() {
    char board[3][3];
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) board[i][j] = ' ';
    for (int i = 0; i < my_moves_no; i++) {
        int m = my_moves[i] - 1;
        board[m / 3][m % 3] = my_figure;
    }
    for (int i = 0; i < op_moves_no; i++) {
        int m = op_moves[i] - 1;
        board[m / 3][m % 3] = op_figure;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("|%c", board[i][j]);
        }
        puts("|");
    }
}

void check_game(int last_move) {
    char winning = ' ';
    char board[3][3];
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) board[i][j] = ' ';
    for (int i = 0; i < my_moves_no; i++) {
        int m = my_moves[i] - 1;
        board[m / 3][m % 3] = my_figure;
    }
    for (int i = 0; i < op_moves_no; i++) {
        int m = op_moves[i] - 1;
        board[m / 3][m % 3] = op_figure;
    }
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][0] == board[i][2]) {
            winning = board[i][0];
        }
        if (board[0][i] == board[1][i] && board[0][i] == board[2][i]) {
            winning = board[0][i];
        }
    }
    //diagonals
    if (board[0][0] == board[1][1] && board[0][0] == board[2][2]) {
        winning = board[0][0];
    }
    if (board[0][2] == board[1][1] && board[0][2] == board[2][0]) {
        winning = board[0][2];
    }
    
    char end_msg[3];
    if (winning != ' ') {
        puts("winner");
        sprintf(end_msg, "L%d" , last_move);
        if(send(server_socket2, end_msg, sizeof(end_msg), 0) == -1) {
            perror("loser msg sned error");
        }
        exit(0);
    }
    if (my_moves_no + op_moves_no == 9) {
        puts("dead draw");
        sprintf(end_msg, "D%d" , last_move);
        if(send(server_socket2, end_msg , sizeof(end_msg), 0) == -1) {
            perror("draw msg sending error");
        }
        exit(0);
    }
}

void make_move() {
    int m;
    while (1) {
        int correct_move = 1;
        printf("give move [square number, are numbered from 1 to 9 from the top left corner to bottom right one]: ");
        scanf("%d", &m);
        if(1 > m || m > 9) {
            puts("wrong val");
            correct_move = 0;
        }
        for (int i = 0; i < my_moves_no; i++) {
            if (my_moves[i] == m) {
                puts("this square is already taken");
                correct_move = 0;
            }
        }

        for (int i = 0; i < op_moves_no; i++) {
            if (op_moves[i] == m) {
                puts("this square is already taken");
                correct_move = 0;
            }
        }
        if(correct_move) {
            break;
        }
    }
    

    my_moves[my_moves_no++] = m;
    display();
    check_game(m);
    char buff[MSG_LEN];
    sprintf(buff, "%d", m);
    if(send(server_socket, buff, sizeof(buff), 0) == -1) {
        perror("move msg sending error");
    }
    puts("###################");
}


void server_listen() {
    printf("server scoket %d\n", server_socket);
    char msg[MSG_LEN];

    struct pollfd *sockets = malloc(sizeof(struct pollfd));
    sockets->fd = server_socket;
    sockets->events = POLLIN;

    my_moves_no = 0;
    op_moves_no = 0;

    while (1 == 1) {
        poll(sockets, 1, -1);
        int bytes_read;
        if((bytes_read = recv(server_socket, msg, MSG_LEN, 0)) == -1) {
            perror("reading from server error");
        }
        msg[bytes_read] = '\0';
        if (strcmp(msg, "P") == 0) {
            //puts("pong");
            if(send(server_socket, "P", sizeof("P"), 0) == -1) {
                perror("ping sending error");
            }
            continue;
        } else if (strcmp(msg, "name taken") == 0) {
            puts("Name is already taken");
            exit(0);
        } else if (strcmp(msg, "no oponnent") == 0) {
            my_moves_no = 0;
            op_moves_no = 0;
            puts("no opponent, for the moment");
        } else if (strcmp(msg, "X") == 0) {
            my_figure = 'X';
            op_figure = 'O';
            printf("my char %c\n", my_figure);
        } else if (strcmp(msg, "O") == 0) {
            my_figure = 'O';
            op_figure = 'X';
            printf("my char %c\n", my_figure);
            display();
            make_move();
        } else if (strlen(msg) == 1 && msg[0] >= '1' && msg[0] <= '9') {
            int move = msg[0] - '0';
            op_moves[op_moves_no++] = move;
            display();
            make_move();
        } else if (strlen(msg) == 2 && msg[0] == 'W') {
            puts("winner");
            exit(0);
        } else if (strlen(msg) == 2 && msg[0] == 'L') {
            op_moves[op_moves_no++] = msg[1] - 48;
            display();
            puts("loser");
            exit(0);
        } else if (strlen(msg) == 2 && msg[0] == 'D') {
            op_moves[op_moves_no++] = msg[1] - 48;
            display();
            puts("dead draw");
            exit(0);
        } else {
            printf("%s\n", msg);
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        puts("wrong number of agrs");
        return 1;
    }
    my_name = argv[1];
    printf("start\n");
    connect_to_server(argv[2], argv[3]);
    server_socket2 = server_socket;
    printf("connected\n");
    register_in_server();
    server_listen();
    return 0;
}