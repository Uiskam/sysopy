#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <sys/ipc.h>

#ifndef SYSOPY_COMM_DEF_H
#define SYSOPY_COMM_DEF_H

#define PIZZA_TYPES_QUAN 10
#define STOVE_SIZE 5
#define TABLE_SIZE 5
#define PATHNAME "/homesysopy"
#define PROJ_ID 2006

#define TABLE_USED 0
#define TABLE_SPACE 1
#define TABLE_NON_EMPTY 2
#define STOVE_USED 3
#define STOVE_SPACE 4


struct Pizzeria_status{
    int stove[STOVE_SIZE];
    int table[TABLE_SIZE];
    int latest_stove_index;
    int latest_table_index;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo  *__buf;
};

#endif //SYSOPY_COMM_DEF_H
