#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <stdbool.h>

#define SECOND 1000000
#define ELVES_NUMBER 10
#define MIN_ELVES_TO_ACTION 3
#define REINDEER_NUMBER 9

void go_to_sleep(int min_sleep, int range) {
    usleep(rand() % (SECOND * range) + SECOND * min_sleep);
}

int reindeers_counter = 0;
int elves_queue_size = 0;
bool can_elf_work[ELVES_NUMBER];

pthread_cond_t elves_are_back = PTHREAD_COND_INITIALIZER;
pthread_mutex_t elves_are_back_mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t elves_sem;

void* elf(void * ID) {
    int id = *((int *)ID);
    printf("elfe with id: %d comes to life\n",id);
    while (1) {
        go_to_sleep(2,3);
        can_elf_work[id] = 0;
        int sem_ret;
        if((sem_ret = sem_trywait(&elves_sem)) == 0) {
            printf("Elf %d: czeka %d elfów na Mikołaja\n",id, ++elves_queue_size);
        } else (sem_ret == -1 && errno == EAGAIN) {
            printf("Elf %d: czeka na powrót elfów\n");
            if(sem_wait((&elves_sem)) != 0){printf("Elf %d sem error",id); perror(""); return -1;}
            printf("Elf %d: czeka %d elfów na Mikołaja\n",id, ++elves_queue_size);
        } else {
            printf("Elf %d queueing error",id);
            perror("");
            return -1;
        }
        pthread_mutex_lock(&elves_are_back_mutex);
        while (can_elf_work[id] == 0){
            pthread_cond_wait(&elves_are_back, &elves_are_back);
        }
        pthread_mutex_unlock(&elves_are_back_mutex);
        printf("Elf %d: wracam do pracy!\n",id);
    }
    
}
int main() {
    if(sem_init(&elves_sem, 0, MIN_ELVES_TO_ACTION) == 0) {
        perror("elves sem creation error");
        return 0;
    }
    for(int i = 0; i < ELVES_NUMBER; i++) can_elf_work[i] = 1;
}