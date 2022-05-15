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
void print_text(char* msg) {
    printf("revcadasd %s\n", msg);
}
int main() {
    struct passwd *pw = getpwuid(getuid());
    char *homedir = pw->pw_dir;
    char* token = strtok(homedir, "/");
    char jamroz_chuj[100] = "/";
    while (token != NULL) {
        strcat(jamroz_chuj, token);
        token = strtok(NULL, "/");
    }
    printf("namne : %s\n", jamroz_chuj);

    return 0;
    /*char msg2[10000]  = "1;longer msg;";
    char* token = strtok(msg2,";");
    printf("token: %s\n",token);
    token = strtok(NULL, ";");
    printf("token2 %s\n",token);
    return 0;
    int actv_users[] = {1,1,1,1,-1,-1,1,1,-1,1};
    int cli_list[22];
    char msg[8192] = "";
    for(int i = 0; i < 10; i++) {
        char tmp[5];
        sprintf(tmp,"%d;",actv_users[i]);
        strcat(msg,tmp);
        //printf("%d\t%s\n",i,msg);
    }
    printf("msg to send %s\n", msg);*/
}