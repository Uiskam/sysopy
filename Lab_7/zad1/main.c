#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include "comm_def.h"
pid_t * producers_pid;
pid_t * delivery_pid;
int producer_nb;
int delivery_nb;
int shm_mem_id;
int sem_set;

void at_exit() {
    for(int i = 0; i < producer_nb; i++) {
        kill(producers_pid[i], SIGINT);
    }
    for(int i = 0; i < delivery_nb; i++) {
        kill(delivery_pid[i], SIGINT);
    }
    free(producers_pid);
    free(delivery_pid);
    if(shmctl(shm_mem_id, IPC_RMID,NULL )==-1){
        perror("shared memory removal error");
    }
    if(semctl(sem_set, 0, IPC_RMID)==-1) {
        perror("sem set removal error");
    }

}
int main(int argc, char **argv) {
    if (argc != 3) {
        puts("Wrong arg number (int)makers_nb (int)delivery_nb are requierd!");
        return -1;
    }
    producer_nb = atoi(argv[1]);
    delivery_nb = atoi(argv[2]);
    if (producer_nb <= 0 || delivery_nb <= 0) {
        puts("makers_nb and deliver_nb must be a positive integer!");
        return -1;
    }

    //shm_mem creation
    key_t mem_key = ftok(PATHNAME, PROJ_ID);
    if (mem_key == -1) {
        perror("mem_key creation error");
        exit(0);
    }
    shm_mem_id = shmget(mem_key, sizeof(struct Pizzeria_status), IPC_CREAT | 0666);
    if (shm_mem_id == -1) {
        perror("shared memory creation creation error");
        exit(0);
    }
    struct Pizzeria_status *pizzeria_status = shmat(shm_mem_id, NULL, 0);
    if (pizzeria_status == (void *) -1) {
        perror("shared memory attaching error");
        exit(0);
    }
    for (int i = 0; i < STOVE_SIZE; i++) pizzeria_status->stove[i] = -1;
    for (int i = 0; i < TABLE_SIZE; i++) pizzeria_status->table[i] = -1;
    pizzeria_status->latest_stove_index = STOVE_SIZE - 1;
    pizzeria_status->latest_table_index = TABLE_SIZE - 1;
    if (shmdt(pizzeria_status) == -1) {
        perror("post initialisation shm memory detaching error");
        exit(0);
    }

    //sem set creation
    key_t sem_set_key = ftok(PATHNAME, PROJ_ID);
    if (sem_set_key == -1) {
        perror("semaphore set key creation error");
        exit(0);
    }
    sem_set = semget(sem_set_key, 5, IPC_CREAT);
    if (sem_set == -1) {
        perror("sem set creation error");
        exit(0);
    }
    union semun arg;
    arg.val = 1;
    if (semctl(sem_set, TABLE_USED, SETVAL, arg) == -1) {
        perror("semaphore TABLE_USED could not be set");
        exit(0);
    }
    if (semctl(sem_set, STOVE_USED, SETVAL, arg) == -1) {
        perror("semaphore STOVE_USED could not be set");
        exit(0);
    }
    arg.val = 0;
    if (semctl(sem_set, TABLE_NON_EMPTY, SETVAL, arg) == -1) {
        perror("semaphore TABLE_USED could not be set");
        exit(0);
    }
    arg.val = STOVE_SIZE;
    if (semctl(sem_set, STOVE_SPACE, SETVAL, arg) == -1) {
        perror("semaphore STOVE_SPACE could not be set");
        exit(0);
    }
    arg.val = TABLE_SIZE;
    if (semctl(sem_set, TABLE_SIZE, SETVAL, arg) == -1) {
        perror("semaphore TABLE_SIZE could not be set");
        exit(0);
    }

    producers_pid = calloc(producer_nb, sizeof(pid_t));
    delivery_pid =calloc(delivery_nb, sizeof(pid_t));
    if(producers_pid == NULL || delivery_pid == NULL) {
        perror("producers/delivery pid allocation error");
        exit(-1);
    }
    for(int i =0 ; i < producer_nb; i++) {
        if((producer_nb[i] = fork()) == -1) {
            printf("%d producer failed to create\n",i);
            perror("");
        } else if(producer_nb[i] == 0) {
            execl("./producer","producer");
            perror("producer run error");
        }
    }
    for(int i =0 ; i < delivery_nb; i++) {
        if((delivery_pid[i] = fork()) == -1) {
            printf("%d delivery failed to create\n",i);
            perror("");
        } else if(delivery_pid[i] == 0) {
            execl("./deliver","deliver");
            perror("deliver run error");
        }
    }
    while (wait(NULL) != -1);
    return 0;
}