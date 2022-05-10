#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/select.h>
#include "comm_def.h"
#include <signal.h>

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
struct comm_msg received_msg;
int server_queue = 0;
FILE *server_hist;

void write_history() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char cur_data[22];
    sprintf(cur_data, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
            tm.tm_min, tm.tm_sec);
    fprintf(server_hist, "on: %s; from: %ld; recieved sig %ld with msg: %s\n", cur_data, received_msg.senderID,received_msg.mtype, received_msg.mtext);
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
    int new_id = find_free_id();
    if (new_id == -1) {
        puts("server capacity exceeded!");
        return;
    }
    active_users[new_id] = 1;
    users_queues[new_id] = atoi(received_msg.mtext);
    if (users_queues[new_id] == -1) {
        printf("client %ld queue opening error", received_msg.senderID);
        perror(" server errINIT ");
        exit(0);
    }
    struct comm_msg server_msg;
    server_msg.mtype = INIT;
    server_msg.senderID = -1;
    sprintf(server_msg.mtext, "%d", new_id);
    if (msgsnd(users_queues[new_id], &server_msg, sizeof(server_msg), 0) < 0) {
        perror("INIT sending error");
        return;
    }
    puts("server sent INIT response");
}

void received_LIST() {
    struct comm_msg server_msg;
    server_msg.mtype = LIST;
    server_msg.senderID = -1;
    printf("active users: ");
    for (int i = 0; i < SERVER_CAPACITY; i++) {
        server_msg.active_users[i] = active_users[i];
        if (active_users[i] != -1) {
            printf("%d ", i);
        }
    }
    printf("\n");
    //printf("sending response to %ld with qid of %d\n",received_msg.senderID,users_queues[received_msg.senderID]);
    if (msgsnd(users_queues[received_msg.senderID], &server_msg, sizeof(server_msg), 0) < 0) {
        perror("list sending error");
        return;
    }
}

void received_2ALL() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char cur_data[22];
    sprintf(cur_data, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
            tm.tm_min, tm.tm_sec);
    struct comm_msg server_msg;
    server_msg.senderID = -1;
    server_msg.mtype = TWOALL;
    sprintf(server_msg.mtext, "msg from: %ld\nreceived on: %s\n%s", received_msg.senderID, cur_data, received_msg.mtext);
    for (int i = 0; i < SERVER_CAPACITY; i++) {
        if (i != received_msg.senderID) {
            if (msgsnd(users_queues[i], &server_msg, sizeof(server_msg), 0) < 0) {
                perror("2ALL server error");
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
    struct comm_msg server_msg;
    server_msg.senderID = -1;
    server_msg.mtype = TWOONE;
    sprintf(server_msg.mtext, "msg from: %ld\nreceived on: %s\n%s", received_msg.senderID, cur_data, received_msg.mtext);
    int receiver_id = atoi(received_msg.mtext);
    if(receiver_id >= 0 && receiver_id < SERVER_CAPACITY && active_users[receiver_id] != -1) {
        if (msgsnd(users_queues[receiver_id], &server_msg, sizeof(server_msg), 0) < 0) {
            perror("2ONE server error");
        }
    }
}

void received_STOP() {
    //puts("UKHTGVHBKMTDTFYGUHJUYTASFDJHSAODSADASDASDASD")
    if(active_users[received_msg.senderID] != -1) {
        active_users[received_msg.senderID] = -1;
        users_queues[received_msg.senderID] = -1;
    } else {
        printf("server received zombie signal!\n");
    }
}

void server_shutdown(int signb) {
    struct comm_msg server_msg;
    server_msg.mtype = SERVER_SHUT_DOWN;
    server_msg.senderID = -1;
    puts("serwer shutting down");
    for(int i = 0; i < SERVER_CAPACITY; i++) {
        if(active_users[i] != -1) {
            if(msgsnd(users_queues[i], &server_msg, sizeof(server_msg), 0) < 0) {
                printf("failed to send stop to %d",i);
                perror(" err ");
                continue;
            }
            msgrcv(users_queues[i], &server_msg, sizeof(server_msg), STOP, 0);
        }
    }
    puts("serwer stopped all of its children");

    if(msgctl(server_queue, IPC_RMID, NULL) != 0) {
        perror("server queue deletion error");
    }
    fclose(server_hist);
    puts("server end work");
    exit(0);
}

int main() {
    signal(SIGINT, server_shutdown);
    server_hist = fopen("server_log.txt", "w");
    if (server_hist == NULL) {
        perror("log open error");
        return -1;
    }
    for (int i = 0; i < SERVER_CAPACITY; i++) active_users[i] = -1;
    server_queue = msgget(SERVER_QUE_KEY, IPC_CREAT | IPC_EXCL | 0666);
    if (server_queue < 0) {
        perror("server queue creation error");
        return -1;
    }
    time_t start,end;
    double dif;
    time(&start);
    while (1) {
        time(&end);
        dif = difftime(end, start);
        if(dif > SERVER_WAIT_TIME){
            puts("server shutting down due to lack of requests");
            server_shutdown(0);
            break;
        }
        if (msgrcv(server_queue, &received_msg, sizeof(received_msg), STOP, IPC_NOWAIT) >= 0) {
            printf("Received signal: %ld from user %ld\n",received_msg.mtype, received_msg.senderID);
            time(&start);
            received_STOP();
            write_history();
        } else if (msgrcv(server_queue, &received_msg, sizeof(received_msg), LIST, IPC_NOWAIT) >= 0) {
            printf("Received signal: %ld from user %ld\n",received_msg.mtype, received_msg.senderID);
            time(&start);
            received_LIST();
            write_history();
        }
        if (msgrcv(server_queue, &received_msg, sizeof(received_msg), 0, IPC_NOWAIT) >= 0) {
            printf("Received signal: %ld from user %ld\n",received_msg.mtype, received_msg.senderID);
            time(&start);
            switch (received_msg.mtype) {
                case INIT:
                    received_INIT();
                    write_history();
                    break;
                case TWOALL:
                    received_2ALL();
                    write_history();
                    break;
                case TWOONE:
                    received_2ONE();
                    write_history();
                    break;
                case STOP:
                    received_STOP();
                    write_history();
                    break;
                case LIST:
                    received_LIST();
                    write_history();
                    break;
                default:
                    printf("server unknown msg type: %ld id: %d \n", received_msg.mtype, -1);
                    write_history();
                    break;
            }
        }
    }
}