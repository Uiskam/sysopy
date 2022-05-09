#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include "comm_def.h"
#include <signal.h>
int check_for_input(void) {
    fd_set rfds;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    printf("select val %d\n",select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv));
    //select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv);
    return (FD_ISSET(0, &rfds));
}

int find_begin_of_msg(const char tmp[MAXMSG + 10]) {
    for(int i = 0; i < strlen(tmp); i++) {
        if(tmp[i] == ';') {
            return i+1;
        }
    }
    return -1;
}
//12;this if test string
int main(){
    printf("%d\n", atoi("-1 "));
    int xd = atoi("12213 21");
    printf("%d\n",xd);
    return 0;
    char cmd[MAXMSG + 10];
    int value = 0;
    fgets(cmd, MAXMSG + 10, stdin);
    value = atoi(cmd);
    char msg[MAXMSG + 10];
    int index = find_begin_of_msg(cmd);
    strncpy(msg,cmd + index,strlen(cmd));
    printf("msg: %s",msg);
    return 0;
    while (1) {
        char tmp[MAXMSG];
        if(scanf("%s",tmp) < 0) return 0;
        printf("%s\n",tmp);
    }
    while (1) {
        if (check_for_input() != 0) {
            char buf[100];
            if(scanf("%s", buf) < 0) {
                break;
            }
            printf("%s\n", buf);
        } else {
            puts("CHUJ LKURAW");
            break;
        }
    }
}