#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <pthread.h>

pthread_t watek01;
pthread_mutex_t mutex01 = PTHREAD_MUTEX_INITIALIZER;

void* fun_watka(void* cos) {
    while (1)
    {
        printf("%s ",(char *)cos);
        fflush(stdout);
        sleep(1);
    }
}

int main() {
    int i;
    pthread_create(&watek01, NULL, &fun_watka, "A");
    pthread_create(&watek01, NULL, &fun_watka, "B");

    for(int i = 1; i < 10000; i++) {
        pthread_mutex_lock (&mutex01);
        printf("%d ",i);
        fflush(stdout);
        pthread_mutex_unlock (&mutex01);
        sleep(3);
    }
}