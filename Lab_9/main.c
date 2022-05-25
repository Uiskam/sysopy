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
    if (usleep(rand() % (SECOND * range) + SECOND * min_sleep)) {
        perror("sleep error!");
    }
}

int reindeer_counter = 0;
int elves_queue_size = 0;
bool can_elf_work[ELVES_NUMBER];

pthread_cond_t elves_are_back = PTHREAD_COND_INITIALIZER;
pthread_cond_t wake_up_santa = PTHREAD_COND_INITIALIZER;
pthread_cond_t delivery_done = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elves_are_back_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_status_check_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elves_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t wake_up_santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t delivery_mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t elves_sem;

pthread_t elves_threads[ELVES_NUMBER];
pthread_t reindeer_threads[REINDEER_NUMBER];

void kill_all_threads() {
    for (int i = 0; i < ELVES_NUMBER; i++) {
        if(pthread_cancel(elves_threads[i])) {
            printf("elf %d cancel error\n",i);
            perror("");
        }
    }
    for (int i = 0; i < REINDEER_NUMBER; i++) {
        if (pthread_cancel(reindeer_threads[i])) {
            printf("cancellation of %d reindeer error", i);
            perror("");
        }
    }
}

void get_into_queue(int id) {
    pthread_mutex_lock(&elves_queue_mutex);
    printf("Elf %d: czeka %d elfów na Mikołaja\n", id, ++elves_queue_size);
    fflush(stdout);
    can_elf_work[id] = 0;
    if (elves_queue_size == 3) {
        pthread_mutex_lock(&wake_up_santa_mutex);
        pthread_cond_broadcast(&wake_up_santa);
        pthread_mutex_unlock(&wake_up_santa_mutex);
    }
    pthread_mutex_unlock(&elves_queue_mutex);
}

void *elf(void *ID) {
    int id = *((int *) ID);
    printf("elf with id: %d comes to life\n", id);
    //go_to_sleep(1, 1);
    while (1) {
        go_to_sleep(2, 3);
        int sem_ret;
        if ((sem_ret = sem_trywait(&elves_sem)) == 0) {
            get_into_queue(id);
        } else if (sem_ret == -1 && errno == EAGAIN) {
            printf("Elf %d: czeka na powrót elfów\n", id);
            fflush(stdout);
            if (sem_wait((&elves_sem)) != 0) {
                printf("Elf %d sem error", id);
                perror("");
                exit(0);
            }
            get_into_queue(id);

        } else {
            printf("ELF %d QUEUEING ERROR!!!", id);
            perror("");
            exit(0);
            return NULL;
        }
        pthread_mutex_lock(&elves_are_back_mutex);
        while (can_elf_work[id] == 0) {
            pthread_cond_wait(&elves_are_back, &elves_are_back_mutex);
        }
        printf("Elf %d: Mikołaj rozwiązuje problem\n", id);
        fflush(stdout);
        pthread_mutex_unlock(&elves_are_back_mutex);
        if (sem_post(&elves_sem) != 0) {
            printf("elf %d destoryed sem", id);
            perror("");
            exit(0);
        }

    }
}

void *reindeer(void *ID) {
    int id = *((int *)ID);
    printf("reinder %d comes to life\n",id);
    while (1) {
        go_to_sleep(5, 5);
        pthread_mutex_lock(&reindeer_status_check_mutex);
        printf("Renifer %d: czeka %d reniferów na Mikołaja\n", id, ++reindeer_counter);
        if (reindeer_counter == 9) {
            printf("Renifer %d: wybudzam Mikołaja\n", id);
            pthread_mutex_lock(&wake_up_santa_mutex);
            pthread_cond_broadcast(&wake_up_santa);
            pthread_mutex_unlock(&wake_up_santa_mutex);
        }
        pthread_mutex_unlock(&reindeer_status_check_mutex);

        pthread_mutex_lock(&delivery_mutex);
        pthread_cond_wait(&delivery_done, &delivery_mutex);
        pthread_mutex_unlock(&delivery_mutex);
    }
}

void *santa(void *null) {
    puts("Santa comes to life\n");
    int delivery_counter = 0;
    while (1) {
        pthread_mutex_lock(&wake_up_santa_mutex);
        pthread_cond_wait(&wake_up_santa, &wake_up_santa_mutex);
        pthread_mutex_unlock(&wake_up_santa_mutex);
        puts("Mikołaj: budzę się");
        pthread_mutex_lock(&reindeer_status_check_mutex);
        pthread_mutex_lock(&elves_queue_mutex);
        if (reindeer_counter == 9) {
            printf("Mikołaj: dostarczam zabawki\n");
            delivery_counter++;
            reindeer_counter = 0;
            go_to_sleep(2,2);
            pthread_cond_broadcast(&delivery_done);
            if(delivery_counter == 3) {
                puts("SANTA IS ON KILLING SPREE!");
                kill_all_threads();
                return NULL;
            }
        } else if (elves_queue_size == 3) {
            printf("Mikołaj: rozwiązuje problemy elfów: ");
            for (int i = 0; i < ELVES_NUMBER; i++) {
                if (can_elf_work[i] == 0) {
                    can_elf_work[i] = 1;
                    printf("%d ", i);
                }
            }
            puts("");
            go_to_sleep(1, 1);
            elves_queue_size = 0;
            pthread_cond_broadcast(&elves_are_back);
        } else if (elves_queue_size > 3 || reindeer_counter > 9) {
            printf("elves queue size should never exceed 3! rn = %d and reindeer counter should never exceed 9 tn = %d\n",
                   elves_queue_size, reindeer_counter);
            kill_all_threads();
            exit(0);
        }
        pthread_mutex_unlock(&reindeer_status_check_mutex);
        pthread_mutex_unlock(&elves_queue_mutex);
        puts("Mikołaj: zasypiam");
    }
}

int main() {
    if (sem_init(&elves_sem, 0, MIN_ELVES_TO_ACTION)) {
        perror("elves sem creation error");
        return 0;
    }

    int elves_id[ELVES_NUMBER], reinder_id[REINDEER_NUMBER];
    for (int i = 0; i < ELVES_NUMBER; i++) {
        can_elf_work[i] = 1;
        elves_id[i] = i;
    }
    for(int i = 0; i < REINDEER_NUMBER; i++) reinder_id[i] = i;
    for (int i = 0; i < ELVES_NUMBER; i++) {
        if (pthread_create(&elves_threads[i], NULL, &elf, (void *) &elves_id[i]) != 0) {
            printf("elf thread %d creation error", i);
            perror("");
        }
    }
    for(int i =0 ; i < REINDEER_NUMBER; i++) {
        //printf("rein i %d id %d\n",i,reinder_id[i]);
        if (pthread_create(&reindeer_threads[i], NULL, &reindeer, (void *) &reinder_id[i]) != 0) {
            printf("reinder thread %d creation error", i);
            perror("");
        }
    }
    pthread_t santa_thread;
    if (pthread_create(&santa_thread, NULL, &santa, NULL)) {
        perror("santa creatiorn error!");
        return 0;
    }
    pthread_join(santa_thread, NULL);
    puts("main work end");
}