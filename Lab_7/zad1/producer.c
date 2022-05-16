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
#include <time.h>
#include <sys/time.h>


int shared_mem_id;
int sem_set;
union semun arg;
struct Pizzeria_status *pizzeria_status;
struct timeval current_time;
struct tm tm;
time_t t;

void exit_handler() {
    if (shmdt(pizzeria_status) == -1) {
        perror("PRODUCENT end  memory detaching error");
        exit(0);
    }
}

void sig_handler(int useless) {
    exit(0);
}

void go_to_sleep(int min_sleep, int range) {
    usleep(rand() % (SECOND * range) + SECOND * min_sleep);
}

void work() {
    struct sembuf table_used;
    table_used.sem_num = TABLE_USED;
    table_used.sem_flg = 0;

    struct sembuf table_space;
    table_space.sem_num = TABLE_SPACE;
    table_space.sem_flg = 0;

    struct sembuf table_non_empty;
    table_non_empty.sem_num = TABLE_NON_EMPTY;
    table_non_empty.sem_flg = 0;

    struct sembuf stove_used;
    stove_used.sem_num = STOVE_USED;
    stove_used.sem_flg = 0;

    struct sembuf stove_space;
    stove_space.sem_num = STOVE_SPACE;
    stove_space.sem_flg = 0;

    int pizza_type = rand() % PIZZA_TYPES_QUAN;
    go_to_sleep(1, 1);

    gettimeofday(&current_time, NULL);
    t = time(NULL);
    tm = *localtime(&t);
    printf("PID: %d on: %d-%02d-%02d %02d:%02d:%02d:%03ld. Przygotowuje pizze: %d\n", getpid(), tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
           tm.tm_min, tm.tm_sec, current_time.tv_usec / 1000, pizza_type);

    stove_space.sem_op = -1;
    stove_used.sem_op = -1;
    if (semop(sem_set, (struct sembuf[]) {stove_space, stove_used}, 2) == -1) {
        perror("PRODUCER locking stove to put in pizza error");
    }
    int my_pizza_index = pizzeria_status->stove_in_index;
    my_pizza_index %= STOVE_SIZE;
    pizzeria_status->stove_in_index++;
    pizzeria_status->stove_in_index %= STOVE_SIZE;
    pizzeria_status->stove[my_pizza_index] = pizza_type;

    int cur_pizza_in_stove = semctl(sem_set, STOVE_SPACE, GETVAL, arg);

    gettimeofday(&current_time, NULL);
    t = time(NULL);
    tm = *localtime(&t);
    printf("PID: %d on: %d-%02d-%02d %02d:%02d:%02d:%03ld. Dodałem pizze: %d Liczba pizz w piecu %d\n", getpid(), tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
           tm.tm_min, tm.tm_sec, current_time.tv_usec / 1000, pizza_type, STOVE_SIZE - cur_pizza_in_stove);

    stove_used.sem_op = 1;
    if(semop(sem_set, &stove_used, 1) == -1) {
        perror("PRODUCER cannot set stove to available");
    }
    go_to_sleep(4, 1);

    stove_used.sem_op = -1;
    table_used.sem_op = -1;
    table_space.sem_op = -1;
    table_non_empty.sem_op = 1;
    if(semop(sem_set, (struct sembuf[]){stove_used, table_used, table_space, table_non_empty}, 4) == -1) {
        perror("PRODUCER cannot get pizza out");
    }
    pizzeria_status->stove[my_pizza_index] = -1;
    pizzeria_status->table[pizzeria_status->table_in_index] = pizza_type;
    pizzeria_status->table_in_index = (pizzeria_status->table_in_index + 1) % TABLE_SIZE;
    cur_pizza_in_stove = semctl(sem_set, STOVE_SPACE, GETVAL, arg);
    int cur_pizza_on_table = semctl(sem_set, TABLE_SPACE, GETVAL, arg);

    gettimeofday(&current_time, NULL);
    t = time(NULL);
    tm = *localtime(&t);
    printf("PID: %d on: %d-%02d-%02d %02d:%02d:%02d:%03ld. Wyjmuje pizze: %d Liczba pizz w piecu %d Liczba pizz na stole %d\n", getpid(), tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
           tm.tm_min, tm.tm_sec, current_time.tv_usec / 1000, pizza_type, STOVE_SIZE - cur_pizza_in_stove - 1, TABLE_SIZE - cur_pizza_on_table);

    stove_used.sem_op = 1;
    table_used.sem_op = 1;
    stove_space.sem_op = 1;
    if(semop(sem_set, (struct sembuf[]){stove_used, table_used, stove_space}, 3) == -1) {
        perror("PRODUCER getting pizza out of stove error");
    }
}

int main() {
    srand(time(NULL) * getpid());
    atexit(exit_handler);
    signal(SIGINT, sig_handler);
    puts("producer start");
    int key = ftok(PATHNAME, PROJ_ID);
    if (key == -1) {
        perror("PRODUCER key gen error");
        return 0;
    }
    shared_mem_id = shmget(key, 0, 0);
    if (shared_mem_id == -1) {
        perror("producent mem open error");
        return 0;
    }
    pizzeria_status = shmat(shared_mem_id, NULL, 0);
    if (pizzeria_status == (void *) -1) {
        perror("PRODUCER memory attaching error");
        exit(0);
    }
    if ((sem_set = semget(key, 0, 0)) == -1) {
        perror("producent sem set open error");
        exit(0);
    }
    while (1) { work(); };
    return 0;


}