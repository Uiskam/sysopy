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
#include <sys/time.h>
#include <time.h>
/**
 * Returns the current time in microseconds.
 */
void getTime(char * cur_time) {
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char cur_data[30];
    char* retVal;
    sprintf(cur_data,"%d-%02d-%02d %02d:%02d:%02d:%03ld", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
           tm.tm_min, tm.tm_sec,current_time.tv_usec / 1000);
    return retVal;
}
const char* myName() {
    char *name = "Flavio";
    return name;
}
int main() {
    printf("%s",getTime());
}