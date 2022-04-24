#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <string.h>
#include <stdbool.h>
/*
    Napisać program przyjmujący jeden (nadawca lub data) lub trzy argumenty (<adresEmail> <tytuł> <treść>):

    W przypadku wywołania z jednym argumentem uruchamiany jest (za pomocą popen()) program mail. 
    Program użytkownika ma wypisywać listę e-maili posortowaną alfabetycznie wg. adresów e-mail (argument nadawca) lub wg. daty otrzymania e-maili (argument data)
    
    Jeżeli program zostanie wywołany z trzema argumentami, to (za pomocą popen()) uruchamiany jest program mail i za jego pomocą wysyłany jest e-mail do określonego nadawcy z określonym tematem i treścią

ari*/


void send_email(char* email, char* topic, char* text) {
    char *command = (char *)malloc(sizeof text  + sizeof topic + sizeof email + sizeof(char) * 64); 
    sprintf(command, "echo -e \"%s\" | mail -s %s %s", text, topic, email);
    FILE* sending_pipe = popen(command, "r");
    if(sending_pipe == NULL) {
        perror("error while creating pipe\n");
        exit(-1);
    }
    pclose(sending_pipe);
    free(command);
}

void print_mails(int mode) {
    char* command;
    if(mode == 0) { //sort by date
        command = "echo | mail -H";
    } else { //sort by sender
        command = "echo | mail -H | sort -k 3";
    }
    FILE* recievied_pipe = popen(command, "r");
    if(recievied_pipe == NULL) {
        perror("error while creating pipe\n");
        exit(1);
    }
    char* mail_recieved;
    size_t mail_info_len;
    while (getline(&mail_recieved, &mail_info_len, recievied_pipe) != -1) {
        printf("%s\n",mail_recieved);
    }
    
    pclose(recievied_pipe);
    free(command);
    free(mail_recieved);
}
int main(int argc, char **argv) {
    switch (argc)
    {
    case 2:
        if(strcmp(argv[1],"nadawca") == 0 || strcmp(argv[1],"data") == 0){
            print_mails(strcmp(argv[1],"data"));
        } else {
            printf("Arg must be \"nadawca\" or \"data\"\n");
            return -1;
        }
        break;
    case 4:
        send_email(argv[1], argv[2], argv[3]);
        printf("msg sent\n");
        break;
    default:
        printf("wrong arg number (1 or 3 are requiered)\n");
        return -1;
    }
    return 0;
}
