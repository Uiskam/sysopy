#include <unistd.h>
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
    if(usleep(rand() % (SECOND * range) + SECOND * min_sleep)) {
        perror("sleep error!");
    }
}

int reindeers_counter = 0;
int elves_queue_size = 0;
bool can_elf_work[ELVES_NUMBER];

pthread_cond_t elves_are_back = PTHREAD_COND_INITIALIZER;
pthread_cond_t wake_up_santa = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elves_are_back_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elves_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elves_queue_check = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeers_status_check = PTHREAD_MUTEX_INITIALIZER;

sem_t elves_sem;
sem_t santa_sleep;

void* elf(void * ID) {
    int id = *((int *)ID);
    printf("elfe with id: %d comes to life\n",id);
    while (1) {
        go_to_sleep(2,3);
        int sem_ret;
        if((sem_ret = sem_trywait(&elves_sem)) == 0) {
            pthread_mutex_lock(&elves_queue_mutex);
            printf("Elf %d: czeka %d elfów na Mikołaja\n",id, ++elves_queue_size);
            pthread_mutex_unlock(&elves_queue_mutex);
            can_elf_work[id] = 0;
            pthread_mutex_lock(&elves_queue_mutex);
            if(elves_queue_size == 3) {
                if(sem_post(&santa_sleep) != 0) {
                    printf("elf %d SANTA IS DEAD!", id);
                    perror("");
                    return NULL;
                }
            }
            pthread_mutex_unlock(&elves_queue_mutex);
        } else if(sem_ret == -1 && errno == EAGAIN) {
            printf("Elf %d: czeka na powrót elfów\n", id);
            if(sem_wait((&elves_sem)) != 0){printf("Elf %d sem error",id); perror(""); return NULL;}
            pthread_mutex_lock(&elves_queue_mutex);
            printf("Elf %d: czeka %d elfów na Mikołaja\n",id, ++elves_queue_size);
            pthread_mutex_unlock(&elves_queue_mutex);
            can_elf_work[id] = 0;
        } else {
            printf("Elf %d queueing error",id);
            perror("");
            return NULL;
        }
        pthread_mutex_lock(&elves_are_back_mutex);
        while (can_elf_work[id] == 0){
            pthread_cond_wait(&elves_are_back, &elves_are_back_mutex);
        }
        pthread_mutex_unlock(&elves_are_back_mutex);
        if(sem_post(&elves_sem) != 0){printf("elf %d destoryed sem", id);perror("");return 0;}
        printf("Elf %d: wracam do pracy!\n",id);
        pthread_mutex_lock(&elves_queue_mutex);
        elves_queue_size--;
        pthread_mutex_unlock(&elves_queue_mutex);
    }
}

void kill_all_threads(){puts("TO BE IMPLEMENTED");}

void* santa(void * null) {
    puts("Santa comes to lifr\n");
    int delivery_counter = 0;
    while (1) {
        if(sem_wait(&santa_sleep) != 0){perror("santa sleep sem_wait error"); kill_all_threads(); return 0;}
        pthread_mutex_lock(&reindeers_status_check);
        pthread_mutex_lock(&elves_queue_check);
        if(reindeers_counter == 9) {
            printf("Mikołaj: dostarczam zabawki\n");
        } else if(elves_queue_size == 3) {
            printf("Mikolaj rozwiazuje problemy elfów: ");
            for(int i = 0; i < ELVES_NUMBER; i++) {
                if(can_elf_work[i] == 0) {
                    can_elf_work[i] = 1;
                    printf("%d ",i);
                }
            }
            puts("");
        } else if(elves_queue_size > 3){printf("elves queue size should never exceed 3! rn = %d\n", elves_queue_size);kill_all_threads(); return NULL;}
        pthread_mutex_unlock(&reindeers_status_check);

    }
}

int main() {
    if(sem_init(&elves_sem, 0, MIN_ELVES_TO_ACTION)) {
        perror("elves sem creation error");
        return 0;
    }
    if(sem_init(&santa_sleep, 0, 0)) {
        perror("santa sem creation error");
        return 0;
    }
    pthread_t elves_threads[ELVES_NUMBER];
    int elves_id[ELVES_NUMBER];
    for(int i = 0; i < ELVES_NUMBER; i++) {
        can_elf_work[i] = 1; 
        elves_id[i] = i;
    }
    //printf("elves nubmer %d\n",ELVES_NUMBER);
    for(int i = 0; i < ELVES_NUMBER; i++) {
        //printf("i = %d id = %d\n",i,elves_id[i]);
        if(pthread_create(&elves_threads[i], NULL, &elf, (void *)&elves_id[i]) != 0) {
            printf("thread %d creation error",i);
            perror("");
        }
    }
    sleep(20);
    puts("main work end");
}