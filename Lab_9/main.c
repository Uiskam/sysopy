#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>


#define SECOND 1000000
#define ELVES_NUMBER 10
#define MIN_ELVES_TO_ACTION 3
#define REINDEER_NUMBER 9

void go_to_sleep(int min_sleep, int range) {
    usleep(rand() % (SECOND * range) + SECOND * min_sleep);
}

int reindeers_counter = 0;
int elves_queue_size = 0;
sem_t elves_sem;

void* elf(void * ID) {
    int id = *((int *)ID);
    printf("elfe with id: %d comes to life\n",id);
    int sem_val;
    while (1) {
        go_to_sleep(2,3);
        
        if(sem_wait(&elves_sem) != 0) {printf("elf %d sem_wait error", id);perror("");}
        printf("Elf %d: czeka %d elfów na Mikołaja\n",id, ++elves_queue_size);
    }
    
}
int main() {
    if(sem_init(&elves_sem, 0, MIN_ELVES_TO_ACTION) == 0) {
        perror("elves sem creation error");
        return 0;
    }

}