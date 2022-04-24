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
int main(int argc, char **argv) {
    switch (argc)
    {
    case 2:
        printf("2\n");
        break;
    case 3:
        char *command; 
        
        break;
    default:
        printf("wrong arg number (2 or 3 are requiered)\n");
        return -1;
    }
    return 0;
}