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
    case 4:
        if(!isnumber(argv[1])){
            puts("Producers number must be a postivie int");
            return -1;
        }
        int number_of_producers = atoi(argv[1]);
        //printf("nb of prod %d\n",number_of_producers);
        for(int i = 1; i <= number_of_producers; i++) {
            //puts("XDD");
            if(fork() == 0){
                char prod_nb[5], prod_input_name[10];
                sprintf(prod_nb,"%d",i);
                sprintf(prod_input_name,"%d.in",i);
                execl("./prod","./prod",argv[2],prod_nb,prod_input_name,argv[3],NULL);
                break;
            }
        }
        execl("./kons","./kons",argv[2],"result.out",argv[3],NULL);
        break;
    
    default:
        printf("there must be 3 arg (number of proucers, fifo name, N)\n");
        return -1;
        break;
    }
}