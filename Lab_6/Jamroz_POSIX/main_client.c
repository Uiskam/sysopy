#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <mqueue.h>
#define MAX_SIZE 2048

int main(int argc, char** argv) {

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    char* name = "/tmpqueue";
    if (mq_unlink(name) != 0)
        perror("XD");
    int q_id = mq_open(name, O_WRONLY,0666, &attr);
    char msg[MAX_SIZE];
    for (long i=0; i<20; i++) {
        sprintf(msg, "mag nb: %d", i);
        printf("sending (msg: %s, prio: %ld) size %d\n", msg, i, MAX_SIZE);
        int result = mq_send(q_id, msg, MAX_SIZE, i);
        if (result < 0) {
            perror("msgsnd");
            return 1;
        }
    }

    return 0;
}
