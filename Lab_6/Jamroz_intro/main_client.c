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

    int queue_id = msgget(key, IPC_CREAT | 0600);

    packet_st p;
    p.type = 1;

    for (long i=0; i<20; i++) {
        p.msg = (char) i;
        printf("sending (%ld, %d) size %lu\n", p.type, p.msg, sizeof(p));
        int retval = msgsnd(queue_id, &p, sizeof(p), 0);
        if (retval < 0) {
            perror("msgsnd");
            return 1;
        }
    }

    return 0;
}
