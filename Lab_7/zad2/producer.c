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
}

void sig_handler(int useless) {
    exit(0);
}

void go_to_sleep(int min_sleep, int range) {
    usleep(rand() % (SECOND * range) + SECOND * min_sleep);
}

void work() {
    int pizza_type = rand() % PIZZA_TYPES_QUAN;
    go_to_sleep(1, 1);
    gettimeofday(&current_time, NULL);
    t = time(NULL);
    tm = *localtime(&t);
    printf("PID: %d on: %d-%02d-%02d %02d:%02d:%02d:%03ld. Przygotowuje pizze: %d\n", getpid(), tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
           tm.tm_min, tm.tm_sec, current_time.tv_usec / 1000, pizza_type);

    if (sem_wait(stove_space) == -1 || sem_wait(stove_used) == -1) {
        perror("PRODUCER locking stove to put in pizza error");
    }
    int my_pizza_index = pizzeria_status->stove_in_index;
    my_pizza_index %= STOVE_SIZE;
    pizzeria_status->stove_in_index++;
    pizzeria_status->stove_in_index %= STOVE_SIZE;
    pizzeria_status->stove[my_pizza_index] = pizza_type;

    int cur_pizza_in_stove;
    if(sem_getvalue(stove_space, &cur_pizza_in_stove) == -1) perror("PRODUCER sem read err");
    gettimeofday(&current_time, NULL);
    t = time(NULL);
    tm = *localtime(&t);
    printf("PID: %d on: %d-%02d-%02d %02d:%02d:%02d:%03ld. Dodałem pizze: %d Liczba pizz w piecu %d\n", getpid(), tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
           tm.tm_min, tm.tm_sec, current_time.tv_usec / 1000, pizza_type, STOVE_SIZE - cur_pizza_in_stove);

    if(sem_post(stove_used) == -1) perror("PRODUCER cannot set stove to available");

    go_to_sleep(4, 1);


    if(sem_wait(stove_used) == -1 || sem_wait(table_used) == -1|| sem_wait(table_space) == -1|| sem_post(table_non_empty) == -1) {
        perror("PRODUCER cannot get pizza out");
    }
    pizzeria_status->stove[my_pizza_index] = -1;
    pizzeria_status->table[pizzeria_status->table_in_index] = pizza_type;
    pizzeria_status->table_in_index = (pizzeria_status->table_in_index + 1) % TABLE_SIZE;
    sem_getvalue(stove_space, &cur_pizza_in_stove);
    int cur_pizza_on_table;
    sem_getvalue(table_space, &cur_pizza_on_table);
    gettimeofday(&current_time, NULL);
    t = time(NULL);
    tm = *localtime(&t);
    printf("PID: %d on: %d-%02d-%02d %02d:%02d:%02d:%03ld. Wyjmuje pizze: %d Liczba pizz w piecu %d Liczba pizz na stole %d\n", getpid(), tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
           tm.tm_min, tm.tm_sec, current_time.tv_usec / 1000, pizza_type, STOVE_SIZE - cur_pizza_in_stove - 1, TABLE_SIZE - cur_pizza_on_table);

    if(sem_post(stove_used) || sem_post(table_used) || sem_post(stove_space)) {
        perror("PRODUCER getting pizza out of stove error");
    }
}

int main() {
    srand(time(NULL));
    atexit(exit_handler);
    signal(SIGINT, sig_handler);
    puts("producer alived");
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