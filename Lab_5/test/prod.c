#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <sys/file.h>


int main(int argc, char** argv) {
    FILE* production_output = fopen("pipe", "w");
    if(production_output == NULL) {
        perror("Pipe open fail!");
        exit(1);
    }

    FILE* supply = fopen("tmp", "r");
    if(supply == NULL) {
        perror("Supply file opening eroor");
        exit(1);
    }
    char* buffer = (char *)malloc(sizeof(char) * 1000);
    if(buffer == NULL) {
        perror("Buffer allocation error");
        exit(1);
    }
    int chars_read;
    int N = 1;
    while ((chars_read = fread(buffer,sizeof(char),N,supply))) {
        struct timespec tim;
        tim.tv_sec = 1;
        tim.tv_nsec = (rand()%1000)*1000000;
        if(nanosleep(&tim, NULL) < 0 ) {
            printf("Nanosleep error\n");
            exit(1);
        }
        puts("XDD");
        flock(fileno(production_output), LOCK_EX);
        fwrite(buffer, sizeof(char),N,production_output);
        flock(fileno(production_output), LOCK_UN);
    }
    struct timespec tim;
        tim.tv_sec = 1;
        tim.tv_nsec = (rand()%1000)*1000000;
        if(nanosleep(&tim, NULL) < 0 ) {
            printf("Nanosleep error\n");
            exit(1);
        }
    free(buffer);
    fclose(production_output);
    fclose(supply);
}