#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include "comm_def.h"
#include <signal.h>
/*
 * Klient wysyła zlecenia, które zawierają:
 *  - typ wiadomości, typ zlecenia
 *  - treść, ID klienta
 *
 * Zlecenia KAŻDE zlecenie ma zawierać id klienta, od którego pochodząi jego rodzaj:
 *  - INIT
 *    klient:
 *      wysyła ID jego kolejki na serwer
 *    serwer:
 *      otwiera zadaną kolejkę
 *      wysyła klientowi przypisane mu ID (przez kolejke klienta)
 *  otwarcie kolejki klienta (nr w kolejności zgłoszeń) i odsyła mu to ID
 *    (komunikacja ta zachodzi przez kolejke klienta)
 *  - LIST done
 *    serwer:
 *      wypisanie aktywnych klientów
 *  - 2ALL string done
 *    klient:
 *      wiadomość
 *    serwer:
 *      przekazuje do wszystkich innych wiadomość, ID nadawcy i aktualna datę
 *  - 2ONE id_klient string, done
 *    klient:
 *      ID adresata (z listy od serwera)
 *      wiadomość
 *    serwer:
 *      wysyła do adresata wiadomość, ID nadawcy i aktualną datę
 *  - STOP (jest wysyłany również w reakcji na SIGINT)
 *    klient:
 *      wysyła ten komunikat
 *      usuwa swoją kolejkę
 *      kończy pracę
 *    serwer:
 *      usuwa klienta z listy aktywnych
 *      usuwa kolejkę klienta z listy
 *
 * Po uruchomieniu klient:
 *  - tworzy kolejkę IPC (z unikalnym kluczem)
 *  - wysyła jej klucz komunikatem do serwera (komunikat INIT)
 *
 *  Po tym klient może wysyłać zlecenia na serwer (czytane ze stdin), (czyta typy komunikatów)
 */
//const key_t server_queue = SERVER_QUE_KEY;
int my_id = -1;
int my_queue = 0;
int init_ID = 0;
struct comm_msg received_msg;
int active_users[SERVER_CAPACITY];
int server_queue = 0;

void send_STOP(); //declaration

void intHandler(int sig_number) {
    printf("SIGINT WORK END id: %d\n", init_ID);
    send_STOP();
    exit(0);
}

void send_INIT() {
    struct comm_msg my_msg;
    my_msg.mtype = INIT;
    my_msg.senderID = init_ID;
    sprintf(my_msg.mtext, "%d", my_queue);
    if (msgsnd(server_queue, &my_msg, sizeof(my_msg), 0) < 0) {
        printf("INIT error id: %d", init_ID);
        perror("errorINIT");
        exit(-1);
    }
}

void send_LIST() {
    struct comm_msg my_msg;
    my_msg.mtype = LIST;
    my_msg.senderID = my_id;
    sprintf(my_msg.mtext,"LIST request");
    if (msgsnd(server_queue, &my_msg, sizeof(my_msg), 0) < 0) {
        printf("LIST error id: %d\n", init_ID);
        perror("errorLIST");
        exit(-1);
    }
}

void send_2ALL(char *message) {
    struct comm_msg my_msg;
    my_msg.mtype = TWOALL;
    my_msg.senderID = my_id;
    sprintf(my_msg.mtext, "%s", message);
    if (msgsnd(server_queue, &my_msg, sizeof(my_msg), 0) < 0) {
        printf("2ALL error id: %d\n", init_ID);
        perror("error2ALL");
        exit(-1);
    }
}

void send_2ONE(int receiver_id, char *message) {
    struct comm_msg my_msg;
    my_msg.mtype = TWOONE;
    my_msg.senderID = my_id;
    if(receiver_id < 0 || receiver_id >= SERVER_CAPACITY || active_users[receiver_id] == -1) {
        printf("receiver do not exist id: %d\n",init_ID);
        return;
    }
    sprintf(my_msg.mtext, "%d;%s", receiver_id, message);
    if (msgsnd(server_queue, &my_msg, sizeof(my_msg), 0) < 0) {
        printf("2ONE error id: %d\n", init_ID);
        perror("error2ONE");
        exit(-1);
    }
}

void send_STOP() {
    struct comm_msg my_msg;
    my_msg.mtype = STOP;
    my_msg.senderID = my_id;
    sprintf(my_msg.mtext, "STOP");
    if (msgsnd(server_queue, &my_msg, sizeof(my_msg), 0) < 0) {
        printf("STOP error id: %d\n", init_ID);
        perror("errorSTOP");
        exit(-1);
    }
    sleep(1);
    /*if (msgctl(my_queue, IPC_RMID, NULL) < 0) {
        printf("delete queue error id: %d ", init_ID);
        printf("CLIENT QUE ID %d\n",my_queue);
        perror("client errorDELQUE");
        exit(-1);
    }*/
    printf("WORK END id: %d\n", init_ID);
    exit(0);
}

void received_INIT() {
    my_id = atoi(received_msg.mtext);
    printf("user id: %d has logged in\n", my_id);
}

void received_LIST(int *user_list) {
    //printf("client list of active users: ");
    for (int i = 0; i < SERVER_CAPACITY; i++) {
        user_list[i] = received_msg.active_users[i];
        /*if(user_list[i] != -1) {
            printf("%d ",i);
        }*/
    }
    //puts("");

}

void received_2ALL_2ONE() {
    printf("client with id: %d\n", my_id);
    printf("received msg:\n%s\n\n", received_msg.mtext);
}

void received_SERVER_SHUTD_DOWN() {
    send_STOP();
}

int stdin_nonempty(void) {
    fd_set rfds;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);
    return select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv);
    //return (FD_ISSET(0, &rfds));
}

int find_begin_of_msg(const char input_str[MAXMSG + 10]) {
    for (size_t i = 0; i < strlen(input_str); i++) {
        if (input_str[i] == ';') {
            return i + 1;
        }
    }
    return -1;
}

void close_at_exit() {
    if(msgctl(my_queue, IPC_RMID, NULL) < 0) {
        perror("close at exit error!!!!");
    }
}
int main(int argc, char **argv) {
    atexit(close_at_exit);
    for (int i = 0; i < SERVER_CAPACITY; i++) active_users[i] = -1;
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    signal(SIGINT, intHandler);
    if (argc != 2) {
        printf("wrong arg number, 1 arg (int) is required\n");
        return -1;
    }
    init_ID = atoi(argv[1]);
    key_t key = ftok(homedir, init_ID);
    my_queue = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if(my_queue < 0) {
        perror("queue could not be opened");
        return -1;
    }
    server_queue = msgget(SERVER_QUE_KEY, 0);
    if (server_queue < 0) {
        perror("server queue opening error");
        return -1;
    }
    //printf("CLIENT QUE CHECK BEFOR INIT ID %d\n",my_queue);
    send_INIT();
    msgrcv(my_queue, &received_msg, sizeof(received_msg), INIT, 0);
    received_INIT();
    //printf("CLIENT QUE CHECK AFTER INIT ID %d\n",my_queue);


    while (1) {
        //puts("nap");
        sleep(1);
        //puts("WROK begin");
        if (msgrcv(my_queue, &received_msg, sizeof(received_msg), STOP, IPC_NOWAIT) >= 0) {
            send_STOP();
        } else if (msgrcv(my_queue, &received_msg, sizeof(received_msg), LIST, IPC_NOWAIT) >= 0) {
            received_LIST(active_users);
        }
        if (msgrcv(my_queue, &received_msg, sizeof(received_msg), 0, IPC_NOWAIT) >= 0) {
            switch (received_msg.mtype) {
                case LIST:
                    received_LIST(active_users);
                    break;
                case TWOALL:
                    received_2ALL_2ONE();
                    break;
                case TWOONE:
                    received_2ALL_2ONE();
                    break;
                case SERVER_SHUT_DOWN:
                    received_SERVER_SHUTD_DOWN();
                    break;
                default:
                    printf("client unknown msg type: %ld id: %d \n", received_msg.mtype, init_ID);
                    break;
            }
        }
        char cmd[MAXMSG + 10];
        char msg_input[MAXMSG + 10];
        if (stdin_nonempty() && fgets(cmd, MAXMSG + 10, stdin) != NULL) {
            int request_number = atoi(cmd);
            int msg_begin = find_begin_of_msg(cmd);
            if(msg_begin == -1){
                puts("wrong instruction");
                continue;
            }
            strncpy(msg_input, cmd + msg_begin, strlen(cmd));

            if(request_number == LIST) {
                send_LIST();
            }
            else if (request_number == TWOALL) {
                send_2ALL(msg_input);
            }
            else if (request_number == TWOONE) {
                int receiver_id = atoi(msg_input);
                char msg_to_send[MAXMSG + 10];
                msg_begin = find_begin_of_msg(msg_input);
                if (msg_begin == -1) {
                    puts("wrong instruction");
                    continue;
                }
                strncpy(msg_to_send, msg_input + msg_begin, strlen(msg_input));
                send_2ONE(receiver_id, msg_to_send);
            }
            else if (request_number == STOP) {
                send_STOP();
            } else {
                printf("lower unknown msg type: %ld id: %d \n", received_msg.mtype, init_ID);
            }
        }
    }


}