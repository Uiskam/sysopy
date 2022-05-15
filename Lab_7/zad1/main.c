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
#include "comm_def.h"

int *producers_pid = NULL;
int *delivery_pid = NULL;
int shm_mem_id;
int sem_set;

void exit_handling() {
    if(producers_pid != NULL) {
        for (int i = 0; i < PRODUCER_NB; i++) {
            kill(producers_pid[i], SIGINT);
        }
    }
    if(delivery_pid != NULL) {
        for (int i = 0; i < DELIVERY_NB; i++) {
            kill(delivery_pid[i], SIGINT);
        }
    }

    if (shmctl(shm_mem_id, IPC_RMID, NULL) == -1) {
        perror("MAIN shared memory removal error");
    }
    if (semctl(sem_set, 0, IPC_RMID) == -1) {
        perror("MAIN sem set removal error");
    }
    puts("main end");
}

void sig_handler(int sig_nb) {
    exit(0);
}

int main() {
    atexit(exit_handling);
    signal(SIGINT, sig_handler);

    //shm_mem creation
    struct passwd *pw = getpwuid(getuid());
    const char *homedir = pw->pw_dir;
    key_t mem_key = ftok(PATHNAME, PROJ_ID);
    if (mem_key == -1) {
        perror("MAIN mem_key creation error");
        exit(0);
    }
    shm_mem_id = shmget(mem_key, sizeof(struct Pizzeria_status), IPC_CREAT | 0666);
    if (shm_mem_id == -1) {
        perror("MAIN shared memory creation creation error");
        exit(0);
    }
    struct Pizzeria_status *pizzeria_status = shmat(shm_mem_id, NULL, 0);
    if (pizzeria_status == (void *) -1) {
        perror("MAIN shared memory attaching error");
        exit(0);
    }
    for (int i = 0; i < STOVE_SIZE; i++) pizzeria_status->stove[i] = -1;
    for (int i = 0; i < TABLE_SIZE; i++) pizzeria_status->table[i] = -1;
    pizzeria_status->stove_in_index = 0;
    pizzeria_status->stove_out_index = 0;
    pizzeria_status->table_in_index = 0;
    pizzeria_status->table_out_index = 0;
    if (shmdt(pizzeria_status) == -1) {
        perror("MAIN post initialisation shm memory detaching error");
        exit(0);
    }

    //sem set creation
    key_t sem_set_key = ftok(PATHNAME, PROJ_ID);
    if (sem_set_key == -1) {
        perror("MAIN semaphore set key creation error");
        exit(0);
    }
    sem_set = semget(sem_set_key, 5, IPC_CREAT | 0666);
    if (sem_set == -1) {
        perror("MAIN sem set creation error");
        exit(0);
    }
    union semun arg;
    arg.val = 1;
    if (semctl(sem_set, TABLE_USED, SETVAL, arg) == -1) {
        perror("MAIN semaphore TABLE_USED could not be set");
        exit(0);
    }
    if (semctl(sem_set, STOVE_USED, SETVAL, arg) == -1) {
        perror("MAIN semaphore STOVE_USED could not be set");
        exit(0);
    }
    arg.val = 0;
    if (semctl(sem_set, TABLE_NON_EMPTY, SETVAL, arg) == -1) {
        perror("MAIN semaphore TABLE_USED could not be set");
        exit(0);
    }
    arg.val = STOVE_SIZE;
    if (semctl(sem_set, STOVE_SPACE, SETVAL, arg) == -1) {
        perror("MAIN semaphore STOVE_SPACE could not be set");
        exit(0);
    }
    arg.val = TABLE_SIZE;
    if (semctl(sem_set, TABLE_SPACE, SETVAL, arg) == -1) {
        perror("MAIN semaphore TABLE_SPACE could not be set");
        exit(0);
    }

    producers_pid = calloc(PRODUCER_NB, sizeof(pid_t));
    delivery_pid = calloc(DELIVERY_NB, sizeof(pid_t));
    if (producers_pid == NULL || delivery_pid == NULL) {
        perror("MAIN producers/delivery pid allocation error");
        exit(-1);
    }
    for (int i = 0; i < PRODUCER_NB; i++) {
        if ((producers_pid[i] = fork()) == -1) {
            printf("%d producer failed to create\n", i);
            perror("");
        } else if (producers_pid[i] == 0) {
            execl("./producer", "producer", (char *) NULL);
            perror("MAIN producer run error");
        }
    }
    for (int i = 0; i < DELIVERY_NB; i++) {
        if ((delivery_pid[i] = fork()) == -1) {
            printf("MAIN %d delivery failed to create\n", i);
            perror("");
        } else if (delivery_pid[i] == 0) {
            execl("./deliver", "deliver", (char *) NULL);
            perror("MAIN deliver run error");
        }
    }
    while (wait(NULL) != -1);
    return 0;
}