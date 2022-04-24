#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <string.h>
/*
    Napisać program przyjmujący jeden (nadawca lub data) lub trzy argumenty (<adresEmail> <tytuł> <treść>):

    W przypadku wywołania z jednym argumentem uruchamiany jest (za pomocą popen()) program mail. 
    Program użytkownika ma wypisywać listę e-maili posortowaną alfabetycznie wg. adresów e-mail (argument nadawca) lub wg. daty otrzymania e-maili (argument data)
    
    Jeżeli program zostanie wywołany z trzema argumentami, to (za pomocą popen()) uruchamiany jest program mail i za jego pomocą wysyłany jest e-mail do określonego nadawcy z określonym tematem i treścią

*/
int send_email(char* email, char* topic, char* text) {
    char *command = malloc(sizeof text  + sizeof topic + sizeof email + sizeof(char) * 32); 
    sprintf(command, "echo -e \"%s\" | mail -s %s %s", text, topic, email);
    FILE* sending_pipe = popen(command, "r");
    if(sending_pipe == NULL) {
        perror("error while creating pipe\n");
        return -1;
    }
    
    char* return_value = malloc(sizeof(char) * 100);
    fgets(return_value, 10, sending_pipe);
    printf("commmunicat %s\n", return_value);
    pclose(sending_pipe);
    free(return_value);
    free(command);
    return 0;
}
int main(int argc, char **argv) {
    switch (argc)
    {
    case 2:
        printf("2\n");
        break;
    case 4:
        send_email(argv[1], argv[2], argv[3]);
        break;
    default:
        printf("wrong arg number (1 or 3 are requiered)\n");
        return -1;
    }
    return 0;
}