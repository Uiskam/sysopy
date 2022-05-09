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

struct comm_msg received_msg;

FILE *server_hist;

void write_history() {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char cur_data[22];
    sprintf(cur_data, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
            tm.tm_min, tm.tm_sec);
    fprintf(server_hist, "on: %s; from: %ld; recieved: %s\n", cur_data, received_msg.senderID, received_msg.mtext);
}

void handler(int tmp) {
    if (fclose(server_hist) != 0) {
        perror("file closing err");
    }
    puts("desturcotr called");
    exit(0);
}

int main() {
    signal(SIGINT, handler);
    server_hist = fopen("server_log.txt", "w");
    if (server_hist == NULL) {
        perror("log open error");
        return -1;
    }
    received_msg.senderID = 0;
    sprintf(received_msg.mtext, "a");
    while (1) {
        write_history();
        sleep(1);
        received_msg.senderID++;
        received_msg.senderID %= SERVER_CAPACITY;
        received_msg.mtext[0]++;
        if(received_msg.mtext[0] >= 123) received_msg.mtext[0] = '1';
        if(received_msg.mtext[0] == ':') received_msg.mtext[0] = 'a';
        puts("log updated");
    }
}