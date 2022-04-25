#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/file.h>
#include <limits.h>
#include <sys/wait.h>
/*
    2 niezależne programy:

    producent:
        - produkuje treści
        - pracuje jego wiele instacji w różnych procesach
        - producent surowce do produkcji bierze z plików, które sa tworzone przez konsumenta
          (kazdy ma osobny plik dla siebie)
        - argumenty:
            - ściezka do potoku nazwanego
            - nr wiersza
            - ściezka do .txt z dowloną zwartością
            - N - liczba znaków odczytywnych na raz
        - działanie:
            - otwiera potok nazwany
            - aż do oczytania całego pliku:
                - odczekuje losowa ilość czasu (rzedu 1-2 s)
                - zapisuje do potokuL nr wiersza oraz odczytany framgent pliku (N znaków)

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

    init:
        -argumenty:
            - liczba producentów
            - nazwa potoku
            - liczba N
            - liczba klientów
        - działanie:
            - uruchamia zadana licbe producenßów i konsumenta

    Oba programy komunikają sie przez potok
*/
int isnumber(const char *number) {
    const char* begin = number;
    if (number == NULL)
        return 1;
    while (*number != '\0') {
        if (!isdigit(*number))
            return 0;
        number++;
    }
    int tmp = atoi(begin);
    if(tmp == 0){
        return 0;
    }
    return 1;
}
int main(int argc, char** argv) {
    switch (argc)
    {
    case 5:
        if(!isnumber(argv[1])){
            puts("Producers number must be a postivie int");
            return -1;
        }
        int number_of_producers = atoi(argv[1]);
        for(int i = 1; i <= number_of_producers; i++) {
            if(fork() == 0){
                char prod_nb[5], prod_input_name[10];
                sprintf(prod_nb,"%d",i*2);
                sprintf(prod_input_name,"./in/%d.in",i);
                execl("./prod","./prod",argv[2],prod_nb,prod_input_name,argv[3],NULL);
                //pipepath, row_nb, inpath, N
                break;
            }
        }
        int number_of_clients = atoi(argv[4]);
        for(int i = 1; i <= number_of_clients; i++){
            if(fork() == 0){
                execl("./kons","./kons",argv[2],"result.out",argv[3],NULL);
            }
        }
        break;
    
    default:
        printf("there must be 4 arg (number of proucers, fifo name, N, client number)\n");
        return -1;
        break;
    }
}