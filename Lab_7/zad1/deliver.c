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
        perror("end DELIVERY memory detaching error");
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


    table_non_empty.sem_op = -1;
    table_used.sem_op = -1;
    if(semop(sem_set, (struct sembuf[]){table_non_empty, table_used}, 2) == -1) {
        perror("DELIVERY table usage lock error");
    }
    int cur_pizza_on_table_nb = semctl(sem_set, TABLE_SPACE, GETVAL);
    int pizza_to_be_delivered = pizzeria_status->table[pizzeria_status->table_out_index];
    gettimeofday(&current_time, NULL);
    t = time(NULL);
    tm = *localtime(&t);
    printf("PID: %d on: %d-%02d-%02d %02d:%02d:%02d:%03ld. Pobieram pizze: %d Liczba pizz na stole %d\n", getpid(), tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
           tm.tm_min, tm.tm_sec, current_time.tv_usec / 1000, pizza_to_be_delivered, TABLE_SIZE - cur_pizza_on_table_nb - 1);
    pizzeria_status->table[pizzeria_status->table_out_index] = -1;
    pizzeria_status->table_out_index = (pizzeria_status->table_out_index + 1) % TABLE_SIZE;
    pizzeria_status->total_delivered++;
    table_used.sem_op = 1;
    table_space.sem_op = 1;
    if(semop(sem_set, (struct sembuf[]){table_used, table_space}, 2) == -1) {
        perror("DELIVERY getting pizza error");
    }
    go_to_sleep(4,1);
    gettimeofday(&current_time, NULL);
    t = time(NULL);
    tm = *localtime(&t);
    printf("PID: %d on: %d-%02d-%02d %02d:%02d:%02d:%03ld. Dostarczam pizze: %d\n", getpid(), tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
           tm.tm_min, tm.tm_sec, current_time.tv_usec / 1000, pizza_to_be_delivered);

    go_to_sleep(4,1);
}
int main() {

    srand(time(NULL) * getpid());
    atexit(exit_handler);
    signal(SIGINT, sig_handler);
    puts("delivery alived");
    int key = ftok(PATHNAME, PROJ_ID);
    if (key == -1) {
        perror("DELIVERY mem key gen error");
        return 0;
    }
    shared_mem_id = shmget(key, 0, 0);
    if (shared_mem_id == -1) {
        perror("producent mem open error");
        return 0;
    }
    pizzeria_status = shmat(shared_mem_id, NULL, 0);
    if (pizzeria_status == (void *) -1) {
        perror("DELIVERY memory attaching error");
        exit(0);
    }

    if ((sem_set = semget(key, 0, 0)) == -1) {
        perror("DELIVERY sem set open error");
        exit(0);
    }
    while (1) { work(); };
    return 0;

}