#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>

/*
    konsument:
        - konsumuje tresci od producentów
        - otrzymane dane od producenta nr i mają się pojwaić w linii nr i pliku wynikowego
        - argumenty:
            - ściezka do potoku
            - sciezka do pliku teksotwego
            - N l. znaków odczytywanych na raz
        - działanie:
            - otwiera potok
            -aż do oczytania cąlego pliku
                - czyta nr wiersza i oraz N znaków z potoku
                - umieszcza odczytane znaki w linii i pliku tekstowego 

*/

int main(int argc, char** argv) {
    switch (argc)
    {
    case 4:
        
        break;
    
    default:
        puts("Wrong arg number (3 are requierd)");
        return -1;
    }
}