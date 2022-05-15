#include <mqueue.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include "comm_def.h"
#include <signal.h>
#include <errno.h>

/*
 * Serwer odpowiada na komunikat klientów.
 * Zlecenia:
 *  - INIT DONE
 *    klient:
 *      wysyła ID jego kolejki na serwer
 *    serwer:
 *      otwiera zadaną kolejkę
 *      wysyła klientowi przypisane mu ID
 *  otwarcie kolejki klienta (nr w kolejności zgłoszeń) i odsyła mu to ID
 *    (komunikacja ta zachodzi przez kolejke klienta)
 *  - LIST DONE
 *    serwer:
 *      wypisanie aktywnych klientów
 *    klient:
 *      akutalizacja listy klientów
 *  - 2ALL string DONE
 *    klient:
 *      wiadomość
 *    serwer:
 *      przekazuje do wszystkich innych wiadomość, ID nadawcy i aktualna datę
 *  - 2ONE id_klient string,
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
 * Priorytety:
 *  - STOP
 *  - LIST
 *  - reszta
 *
 *  Po uruchomieniu:
 *      - tworzy kolejkę, za pomocą której będzie odbierać komunikaty
 *
 *  Po otrzymaniu zlecenia serwer zapisuje do pliku z logiem:
 *  czas otrzymania zlecenia, id klienta i treść komunikatu.
 *
 *  obsługa sygnału CTR + Z == koniec pracy serwera, odebranie STOP od wszystkich klientów, usunięcie kolejki i koniec pracy
 */
int active_users[SERVER_CAPACITY];
int users_queues[SERVER_CAPACITY];
char received_msg[MAX_MSG_SIZE];
char copy_received_msg[MAX_MSG_SIZE];
const char delimiter[2] = ";";
int server_queue = 0;
FILE *server_hist;
char my_queue_name[100];

void write_history() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char cur_data[22];
    char* token;
    token = strtok(copy_received_msg, delimiter);
    int sig_nb = atoi(token);
    token = strtok(NULL, delimiter);
    int sender_id = atoi(token);
    token = strtok(NULL, delimiter);
    sprintf(cur_data, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
            tm.tm_min, tm.tm_sec);
    fprintf(server_hist, "on: %s; from: %d; recieved sig %d with msg: %s\n", cur_data, sender_id,
            sig_nb, token);
}

int find_free_id() {
    for (int i = 0; i < SERVER_CAPACITY; i++) {
        //printf(":user %d status is %d\n",i,active_users[i]);
        if (active_users[i] == -1) {
            return i;
        }
    }
    return -1;
}

void received_INIT() {
    char *token = strtok(received_msg, delimiter);
    token = strtok(NULL, delimiter);
    token = strtok(NULL, delimiter);
    char *client_queue_name = token;

    int new_id = find_free_id();
    if (new_id == -1) {
        puts("server capacity exceeded!");
        return;
    }

    int client_queue_id = mq_open(client_queue_name, O_WRONLY);
    if (client_queue_id < 0) {
        printf("client %s queue opening error", client_queue_name);
        perror("server error, client queue opening");
        exit(0);
    }
    active_users[new_id] = 1;
    users_queues[new_id] = client_queue_id;

    char msg[MAX_MSG_SIZE];
    sprintf(msg, "%d;%d;", INIT, new_id);
    if (mq_send(users_queues[new_id], msg, MAX_MSG_SIZE, 0) < 0) {
        perror("server INIT response error");
        exit(-1);
    }
    puts("server sent INIT response");
}

void received_LIST() {
    char *token = strtok(received_msg, delimiter);
    token = strtok(NULL, delimiter);
    int sender_ID = atoi(token);

    char msg[MAX_MSG_SIZE];
    sprintf(msg, "%d;", LIST);
    printf("active users: ");
    for (int i = 0; i < SERVER_CAPACITY; i++) {
        char tmp[5];
        sprintf(tmp, "%d;", active_users[i]);
        strcat(msg, tmp);
        if (active_users[i] != -1) {
            printf("%d ", i);
        }
    }
    printf("\n");
    if (mq_send(users_queues[sender_ID], msg, MAX_MSG_SIZE, 1) < 0) {
        printf("server list sending error to: %d", sender_ID);
        perror("");
        return;
    }
}

void received_2ALL() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char cur_data[22];
    sprintf(cur_data, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
            tm.tm_min, tm.tm_sec);

    char *token = strtok(received_msg, delimiter);
    token = strtok(NULL, delimiter);
    int sender_ID = atoi(token);
    token = strtok(NULL, delimiter);
    char *message = token;

    char msg[MAX_MSG_SIZE];
    sprintf(msg, "%d;msg from: %d\nreceived on: %s\n%s", TWOALL, sender_ID, cur_data, message);
    for (int i = 0; i < SERVER_CAPACITY; i++) {
        if (i != sender_ID && active_users[i] != -1) {
            if (mq_send(users_queues[i], msg, MAX_MSG_SIZE, 0) < 0) {
                printf("server 2ALL error while sending to %d", i);
                perror("");
            }
        }
    }
}

void received_2ONE() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char cur_data[22];
    sprintf(cur_data, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
            tm.tm_min, tm.tm_sec);

    char *token = strtok(received_msg, delimiter);
    token = strtok(NULL, delimiter);
    int sender_ID = atoi(token);
    token = strtok(NULL, delimiter);
    int receiver_id = atoi(token);
    token = strtok(NULL, delimiter);
    char *message = token;

    char msg[MAX_MSG_SIZE];
    sprintf(msg, "msg from: %d\nreceived on: %s\n%s", sender_ID, cur_data, message);
    if (receiver_id >= 0 && receiver_id < SERVER_CAPACITY && active_users[receiver_id] != -1) {
        if (mq_send(users_queues[receiver_id], msg, MAX_MSG_SIZE, 0) < 0) {
            printf("server 2ONE error while sending to %d", receiver_id);
            perror("");
        }
    }
}

void received_STOP() {
    char *token = strtok(received_msg, delimiter);
    token = strtok(NULL, delimiter);
    int sender_ID = atoi(token);

    if (active_users[sender_ID] != -1) {
        active_users[sender_ID] = -1;
        if (mq_close(users_queues[sender_ID]) < 0) {
            printf("server closing queue error of client: %d\n", sender_ID);
            perror("");
        }
    } else {
        printf("server received zombie signal!\n");
    }
}

void server_shutdown(int signb) {
    char msg[MAX_MSG_SIZE];
    sprintf(msg, "%d;", SERVER_SHUT_DOWN);
    puts("serwer shutting down");
    for (int i = 0; i < SERVER_CAPACITY; i++) {
        if (active_users[i] != -1) {
            if (mq_send(users_queues[i], msg, MAX_MSG_SIZE, 3) < 0) {
                printf("failed to send stop to %d", i);
                perror("");
                continue;
            }
            int bits_read = mq_receive(server_queue, received_msg, MAX_MSG_SIZE, NULL);
            while (bits_read == -1) {
                bits_read = mq_receive(server_queue, received_msg, MAX_MSG_SIZE, NULL);
            }
            received_STOP();
        }
    }
    puts("serwer stopped all of its children");

    if (mq_close(server_queue) < 0) perror("server queue closing error");
    if (fclose(server_hist) < 0) perror("server_log closing error");
    puts("server end work");
    fprintf(stderr, "server end\n");
    exit(0);
}

void close_at_exit() {
    if (mq_unlink(my_queue_name) < 0) {
        printf("queue deleting error server\n");
        perror("");
    } else {
        printf("server queue successfully\n");
    }
}

int create_queue() {
    struct mq_attr atr;
    atr.mq_flags = 0;
    atr.mq_maxmsg = 10;
    atr.mq_msgsize = MAX_MSG_SIZE;
    atr.mq_curmsgs = 0;
    return mq_open(SERVER_QUE_NAME, O_RDONLY | O_CREAT | O_EXCL | O_NONBLOCK, 0666, &atr);
}

int main() {
    printf("server pid %d\n",getpid());
    /*char gowno[100];
    sprintf(gowno, "ps aux | grep %d",getpid());
    system(gowno);*/
    atexit(close_at_exit);
    signal(SIGINT, server_shutdown);
    server_hist = fopen("server_log.txt", "w");
    if (server_hist == NULL) {
        perror("log open error");
        return -1;
    }
    for (int i = 0; i < SERVER_CAPACITY; i++) active_users[i] = -1;
    server_queue = create_queue();
    if (server_queue < 0) {
        if(errno == 17) {
            if(mq_unlink(SERVER_QUE_NAME) < 0){
                perror("existing server queue deletion error");
                return -1;
            }
            server_queue = create_queue();
        }
        if(server_queue < 0) {
            perror("server queue creation error");
            return -1;
        }
    }
    time_t start, end;
    double dif;
    time(&start);
    while (1) {
        //puts("enetering endless loop");
        time(&end);
        dif = difftime(end, start);
        if (dif > SERVER_WAIT_TIME) {
            puts("server shutting down due to lack of requests");
            server_shutdown(0);
            break;
        }
        if (mq_receive(server_queue, received_msg, MAX_MSG_SIZE, NULL) >= 0) {
            puts("enetering endless loop");
            printf("Received signal: %d\n", atoi(received_msg));
            strcpy(copy_received_msg, received_msg);
            time(&start);
            switch (atoi(received_msg)) {
                case STOP:
                    received_STOP();
                    write_history();
                    break;
                case LIST:
                    received_LIST();
                    write_history();
                    break;
                case INIT:
                    puts("SERVER ENTERED RECIEVED INIT");
                    received_INIT();
                    puts("SERVER EXITED RECIEVED INIT");
                    write_history();
                    puts("AIYFGAIYWSDHDOUASHDHSADOIUHSADIUHASDIUHASIUDHASIUDHASIUIUSAHDIUASHDIUASHDIU");
                    break;
                case TWOALL:
                    received_2ALL();
                    write_history();
                    break;
                case TWOONE:
                    received_2ONE();
                    write_history();
                    break;
                default:
                    printf("server unknown msg type: %d id: %d \n", atoi(received_msg), -1);
                    write_history();
                    break;
            }
        }
    }
}