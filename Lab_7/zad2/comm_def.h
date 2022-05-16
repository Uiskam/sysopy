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

#define PRODUCER_NB 1
#define DELIVERY_NB 1

#define TABLE_USED "/table_used"
#define TABLE_SPACE "/table_space"
#define TABLE_NON_EMPTY "/table_non_empty"
#define STOVE_USED "/stove_used"
#define STOVE_SPACE "/stove_space"

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
