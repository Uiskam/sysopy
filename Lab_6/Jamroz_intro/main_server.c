#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef struct packet_s {
    long type;
    char msg;
} packet_st;

int main(int argc, char** argv) {

    key_t key = ftok(".", 'p');
    if (key < 0) {
        perror("ftok");
        return 1;
    }

    int queue_id = msgget(key, 0600);

    while (1) {
        packet_st p;
        int retval = msgrcv(queue_id, &p, sizeof(p), 0, IPC_NOWAIT);
        if (retval < 0) {
            perror("msgrcv");
            break;
        }

        printf("received: %d\n", queue_id);
    }
    puts("HERE");
    msgctl(queue_id, IPC_RMID, NULL);

    return 0;
}
