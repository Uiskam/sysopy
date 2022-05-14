#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include "comm_def.h"
#include <time.h>
#include <signal.h>

int main() {
    char list[100] = "1;2;3;4;5;123;3;3";
    char *token;
    token = strtok(list, ";");
    while (token != NULL) {
        printf("%s\n",token);
        token = strtok(NULL, ";");
    }
}