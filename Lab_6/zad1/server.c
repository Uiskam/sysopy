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
 * Serwer odpowiada na komunikat klientów.
 * Zlecenia:
 *  - INIT
 *    klient:
 *      wysyła ID jego kolejki na serwer
 *    serwer:
 *      otwiera zadaną kolejkę
 *      wysyła klientowi przypisane mu ID
 *  otwarcie kolejki klienta (nr w kolejności zgłoszeń) i odsyła mu to ID
 *    (komunikacja ta zachodzi przez kolejke klienta)
 *  - LIST
 *    serwer:
 *      wypisanie aktywnych klientów
 *    klient:
 *      akutalizacja listy klientów
 *  - 2ALL string
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

int find_free_id() {
    int new_id = -1;
    for(int i = 0; i < SERVER_CAPACITY; i++) {
        if(active_users[i] != -1) {
            return i;
        }
    }
    return i;
}

void received_INIT() {
    int new_id = find_free_id();
    if(new_id == -1) {
        puts("server capacity exceeded!");
        return;
    }
    users_queues[new_id] = msgget(atoi(received_msg.mtext), 0600);
    if(users_queues[new_id] == -1) {
        perror("client %d queue opening error",received_msg.senderID);
    }
    struct comm_msg server_msg;
    server_msg.mtype = INIT;
    server_msg.senderID = -1;
    sprintf(server_msg.mtext,"%d",new_id);
    if(msgsnd(users_queues[new_id], &server_msg, sizeof(server_msg), 0) < 0) {
        perror("INIT sending error");
        return;
    }
}

void received_LIST() {
    struct comm_msg server_msg;
    server_msg.mtype = LIST;
    server_msg.senderID = -1;
    printf("active users: ");
    for(int i = 0; i < SERVER_CAPACITY; i++) {
        server_msg.active_users[i] = active_users[i];
        if(active_users[i] != -1) {
            printf("%d ",i);
        }
        printf("\n");
    }
    if(msgsnd(users_queues[received_msg.senderID], &server_msg, sizeof(server_msg), 0) < 0) {
        perror("list sending error");
        return;
    }z
}

void received_2ALL() {
    struct comm_msg server_msg;
}
int main() {
    for(int i = 0; i < SERVER_CAPACITY; i++) active_users[i] = -1;
    int server_queue = msgget(SERVER_QUE_KEY, IPC_CREAT | IPC_EXCL);
    if (server_queue < 0) {
        if (msgctl(SERVER_QUE_KEY, IPC_RMID, NULL) < 0) {
            perror("crt error");
            return -1;
        }
        server_queue = msgget(SERVER_QUE_KEY, IPC_CREAT | IPC_EXCL);
        if(server_queue < 0) {
            perror("crt error V2");
            return -1;
        }
    }
    while (1) {
        if (msgrcv(my_queue, &received_msg, sizeof(received_msg), STOP, IPC_NOWAIT) >= 0) {
            send_STOP();
        } else if (msgrcv(my_queue, &received_msg, sizeof(received_msg), LIST, IPC_NOWAIT) >= 0) {
            send_LIST();
        }
        if (msgrcv(my_queue, &received_msg, sizeof(received_msg), 0, IPC_NOWAIT) >= 0) {

        }
    }
}