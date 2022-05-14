#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>

#define MAX_SIZE 8192

int main(int argc, char **argv) {

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;


    int q_id = mq_open("/tmpqueue", O_WRONLY | O_CREAT, 0666, &attr);
    if (q_id < 0) {
        perror(":(");
    }
    char msg[MAX_SIZE];
    for (long i = 0; i < 10; i++) {
        sprintf(msg, "mag nb: %d", i);
        printf("sending (msg: %s, prio: %ld) size %d\n", msg, i, MAX_SIZE);
        int result = mq_send(q_id, msg, MAX_SIZE, 0);
        if (result < 0) {
            perror("msgsnd");
            return 1;
        }
    }
    printf("queue_id %d\n",q_id);
    if (mq_close(q_id) < 0){
        perror("on closing");
    }
    /*if (mq_unlink("/tmpqueue") != 0)
        perror("XD");*/
    return 0;
}
