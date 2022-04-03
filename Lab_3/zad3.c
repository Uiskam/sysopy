#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <ctype.h>
#include <time.h>
#include <sys/wait.h>

/*
argumenty: katalog wzorzec_szukany max_głebokość_szukania
działanie: 
- prgoram przeszuje drzewo katalogow w poszukiwaniu plikow (TEKSTOWYCH) zawierających wzorzec_szukany
- podkatalogi przeszukujemy w procesach potomonych
- wypis: ściezka wzgledna do katalogu(jako poczatek traktowany jest arg), PID, zawa pliku
*/
int main(int argc, char **argv) {
    if(argc != 4){
        printf("Wrong args number! Program accepts 4 args:\n\t (dir_name, text_to_find, searching_depth (1 is serachin only through dir_name))\n");
        return -1;
    }
    int search_depth = atoi(argv[3]);
    if(search_depth == 0) {
        printf("serach_depth must be at least 1!\n");
        return -1;
    }
}