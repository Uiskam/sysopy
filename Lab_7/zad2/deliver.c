#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include "comm_def.h"
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

int shm_mem_id;
struct Pizzeria_status *pizzeria_status;
sem_t *table_used;
sem_t *stove_used;
sem_t *table_non_empty;
sem_t *stove_space;
sem_t *table_space;
struct timeval current_time;
struct tm tm;
time_t t;

void exit_handler() {
    if (munmap(pizzeria_status, sizeof(struct Pizzeria_status)) == -1) {
        perror("end DELIVERY memory detaching error");
        exit(0);
    }

    if (sem_close(table_used) == -1) perror("DELIVERY sem close error");
    if (sem_close(stove_used) == -1) perror("DELIVERY sem close error");
    if (sem_close(stove_space) == -1) perror("DELIVERY sem close error");
    if (sem_close(table_space) == -1)perror("DELIVERY sem close error");
    if (sem_close(table_non_empty) == -1)perror("DELIVERY sem close error");
    puts("DELIVER end");
}

void sig_handler(int useless) {
    exit(0);
}

void go_to_sleep(int min_sleep, int range) {
    usleep(rand() % (SECOND * range) + SECOND * min_sleep);
}

void work() {
    if (sem_wait(table_non_empty) == -1 || sem_wait(table_used) == -1) {
        perror("DELIVERY table usage lock error");
    }
    int cur_pizza_on_table_nb;
    if(sem_getvalue(table_space, &cur_pizza_on_table_nb) == -1) perror("DELIVERY sem read err");
    int pizza_to_be_delivered = pizzeria_status->table[pizzeria_status->table_out_index];
    gettimeofday(&current_time, NULL);
    t = time(NULL);
    tm = *localtime(&t);
    printf("PID: %d on: %d-%02d-%02d %02d:%02d:%02d:%03ld. Pobieram pizze: %d Liczba pizz na stole %d\n", getpid(),
           tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
           tm.tm_min, tm.tm_sec, current_time.tv_usec / 1000, pizza_to_be_delivered,
           TABLE_SIZE - cur_pizza_on_table_nb - 1);
    pizzeria_status->table[pizzeria_status->table_out_index] = -1;
    pizzeria_status->table_out_index = (pizzeria_status->table_out_index + 1) % TABLE_SIZE;
    pizzeria_status->total_delivered++;

    if (sem_post(table_used) == -1 || sem_post(table_space) == -1) {
        perror("DELIVERY getting pizza error");
    }
    go_to_sleep(4, 1);
    gettimeofday(&current_time, NULL);
    t = time(NULL);
    tm = *localtime(&t);
    printf("PID: %d on: %d-%02d-%02d %02d:%02d:%02d:%03ld. Dostarczam pizze: %d\n", getpid(), tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
           tm.tm_min, tm.tm_sec, current_time.tv_usec / 1000, pizza_to_be_delivered);

    go_to_sleep(4, 1);
}

int main() {
    puts("delivery start");
    srand(time(NULL) * getpid());
    atexit(exit_handler);
    signal(SIGINT, sig_handler);

    //shm_mem creation
    shm_mem_id = shm_open(PATHNAME, O_RDWR, 0);
    if (shm_mem_id == -1) {
        perror("DELIVERY shared memory creation creation error");
        exit(0);
    }
    pizzeria_status = mmap(NULL, sizeof(struct Pizzeria_status), PROT_READ | PROT_WRITE, MAP_SHARED, shm_mem_id, 0);
    if (pizzeria_status == (void *) -1) {
        perror("DELIVERY shared memory attaching error");
        exit(0);
    }



    //sem set creation
    if ((table_used = sem_open(TABLE_USED, 0)) == SEM_FAILED) {
        perror("DELIVERY sem TABLE_USED error");
        exit(0);
    }
    if ((stove_used = sem_open(STOVE_USED, 0)) == SEM_FAILED) {
        perror("DELIVERY sem STOVE_USED error");
        exit(0);
    }
    if ((table_non_empty = sem_open(TABLE_NON_EMPTY,0)) == SEM_FAILED) {
        perror("DELIVERY sem TBL_NON error");
        exit(0);
    }
    if ((stove_space = sem_open(STOVE_SPACE, 0)) == SEM_FAILED) {
        perror("DELIVERY sem STOVE_SPACE error");
        exit(0);
    }
    if ((table_space = sem_open(TABLE_SPACE, 0)) == SEM_FAILED) {
        perror("DELIVERY sem TABLE_SPACE error");
        exit(0);
    }
    while (1) { work(); };
    return 0;

}