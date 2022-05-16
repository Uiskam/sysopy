#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "comm_def.h"
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int *producers_pid = NULL;
int *delivery_pid = NULL;
int shm_mem_id;
struct Pizzeria_status *pizzeria_status;
sem_t *table_used;
sem_t *stove_used;
sem_t *table_non_empty;
sem_t *stove_space;
sem_t *table_space;

void exit_handling() {
    if (producers_pid != NULL) {
        for (int i = 0; i < PRODUCER_NB; i++) {
            kill(producers_pid[i], SIGINT);
        }
    }
    if (delivery_pid != NULL) {
        for (int i = 0; i < DELIVERY_NB; i++) {
            kill(delivery_pid[i], SIGINT);
        }
    }
    printf("total delivery sum: %d\n", pizzeria_status->total_delivered);
    if (munmap(pizzeria_status, sizeof(struct Pizzeria_status)) == -1) {
        perror("MAIN post initialisation shm memory detaching error");
        exit(0);
    }
    if (shm_unlink(PATHNAME) == -1) {
        perror("MAIN shared memory removal error");
    }

    if (sem_close(table_used) == -1) perror("MAIN sem close error");
    if (sem_close(stove_used) == -1) perror("MAIN sem close error");
    if (sem_close(stove_space) == -1) perror("MAIN sem close error");
    if (sem_close(table_space) == -1)perror("MAIN sem close error");
    if (sem_close(table_non_empty) == -1)perror("MAIN sem close error");

    if (sem_unlink(TABLE_USED) == -1) perror("MAIN sem delete error");
    if (sem_unlink(STOVE_USED) == -1)perror("MAIN sem delete error");
    if (sem_unlink(STOVE_SPACE) == -1) perror("MAIN sem delete error");
    if (sem_unlink(TABLE_SPACE) == -1) perror("MAIN sem delete error");
    if (sem_unlink(TABLE_NON_EMPTY) == -1) perror("MAIN sem delete error");
    puts("main end");
}

void sig_handler(int sig_nb) {
    exit(0);
}

int main() {
    atexit(exit_handling);
    signal(SIGINT, sig_handler);

    //shm_mem creation
    shm_mem_id = shm_open(PATHNAME, O_RDWR | O_CREAT, 0666);
    if (shm_mem_id == -1) {
        perror("MAIN shared memory creation creation error");
        exit(0);
    }
    if (ftruncate(shm_mem_id, sizeof(struct Pizzeria_status)) == -1) {
        perror("MAIN ftruncate error");
        exit(0);
    }
    pizzeria_status = mmap(NULL, sizeof(struct Pizzeria_status), PROT_READ | PROT_WRITE, MAP_SHARED, shm_mem_id, 0);
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
    pizzeria_status->total_delivered = 0;


    //sem set creation
    if ((table_used = sem_open(TABLE_USED, O_CREAT, 0666, 1)) == SEM_FAILED) {
        perror("MAIN sem TABLE_USED error");
        exit(0);
    }
    if ((stove_used = sem_open(STOVE_USED, O_CREAT, 0666, 1)) == SEM_FAILED) {
        perror("MAIN sem STOVE_USED error");
        exit(0);
    }
    if ((table_non_empty = sem_open(TABLE_NON_EMPTY, O_CREAT, 0666, 0)) == SEM_FAILED) {
        perror("MAIN sem TBL_NON error");
        exit(0);
    }
    if ((stove_space = sem_open(STOVE_SPACE, O_CREAT, 0666, STOVE_SIZE)) == SEM_FAILED) {
        perror("MAIN sem STOVE_SPACE error");
        exit(0);
    }
    if ((table_space = sem_open(TABLE_SPACE, O_CREAT, 0666, TABLE_SIZE)) == SEM_FAILED) {
        perror("MAIN sem TABLE_SPACE error");
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