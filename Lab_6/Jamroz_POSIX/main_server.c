#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <stdint.h>
#include <mqueue.h>
#define MAX_SIZE 8192

int main(int argc, char** argv) {


    struct timespec delay;
    delay.tv_sec = 1000000000;
    delay.tv_nsec = 1000;
    int queue_id = mq_open("/tmpqueue",O_RDONLY | O_NONBLOCK);
    if(queue_id < 0) {
        perror("XDDDD");
    }

    while (1) {
        char received[MAX_SIZE];
        unsigned int pri;
        int retval = mq_receive(3, received, MAX_SIZE, &pri);
        if (retval < 0) {
            printf("q_id %d msg legth read %d\n",3,retval);
            perror("mq_receive");
            break;
        }

        printf("received: %s \t with pri of: %d\n", received, pri);
    }
    puts("HERE");
    if (mq_close(queue_id) < 0){
        perror("on closing");
    }

    return 0;
}
