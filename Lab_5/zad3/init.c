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

    Oba programy komunikają sie przez potok
*/
int main() {

}