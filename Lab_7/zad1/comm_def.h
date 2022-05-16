#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/ipc.h>

#ifndef SYSOPY_COMM_DEF_H
#define SYSOPY_COMM_DEF_H

#define PIZZA_TYPES_QUAN 10
#define STOVE_SIZE 5
#define TABLE_SIZE 5
#define PATHNAME "/home"
#define PROJ_ID 2006

#define PRODUCER_NB 30
#define DELIVERY_NB 30

#define TABLE_USED 0
#define TABLE_SPACE 1
#define TABLE_NON_EMPTY 2
#define STOVE_USED 3
#define STOVE_SPACE 4

#define SECOND 1000000


struct Pizzeria_status{
    int stove[STOVE_SIZE];
    int table[TABLE_SIZE];
    int stove_in_index;
    int stove_out_index;
    int table_in_index;
    int table_out_index;
    int total_delivered;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo  *__buf;
};

#endif //SYSOPY_COMM_DEF_H
