#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Precisely one int arg is required!\n");
        return -1;
    }
    int n = atoi(argv[1]);
    if (n <= 0) {
        printf("Postive int is required as arg!\n");
        return -1;
    }
    for (int i = 0; i < n; i++) {
        if(fork() == 0) {
            printf("My PID is %d\n", (int)getpid());
            i = n;
        }
    }
}