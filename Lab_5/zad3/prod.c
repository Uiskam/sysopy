#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
/*
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
*/
int production_out_type_check(char* filepath) {
    struct stat fileinfo;
    if(stat(filepath, &fileinfo) == 0) {
        return !S_ISFIFO(fileinfo.st_mode);
    }
    return -3;
}
int main(int argc, char** argv) {
    switch (argc)
    {
    case 5:;
        FILE* production_output = fopen(argv[1], "W");
        if(production_output == -1) {
            perror("Pipe open fail! ");
            return -1;
        }
        if(production_out_type_check(argv[1])){
            puts("Wrong file type of file given");
            return -1;
        }
        break;
    
    default:
        puts("Wrong arg number (4 are requierd)");
        return -1;
    }
}