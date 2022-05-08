#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include "comm_def.h"
#include <signal.h>

int main(){
    char tmp_text[MAXMSG];
    //tmp_text = "123; jakies dluszy tezcdsafesadc";
    sprintf(tmp_text,"%s","123; jakies dluszy tezcdsafesadc");
    char* prt = tmp_text;
    while (*prt != ';') prt++;prt++;
    printf("data: %s\n",prt);

}