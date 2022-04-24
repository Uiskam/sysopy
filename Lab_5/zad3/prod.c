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
        return S_ISFIFO(fileinfo.st_mode);
    }
    return 0;
}

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

void sub_newlines(char *buf) {
    while (*buf){
        if (*buf == '\n'){
            *buf = '\t';
        }
        buf++;
    }
}

void produce(int row_number, char* supply_path, int N, char* output_name) {
    FILE* production_output = fopen(output_name, "w");
    if(production_output == NULL) {
        perror("Pipe open fail!");
        exit(1);
    }
    if(production_out_type_check(output_name) == 0){
        puts("Wrong file type of file given");
        exit(1);
    }

    FILE* supply = fopen(supply_path, "r");
    if(supply == NULL) {
        perror("Supply file opening eroor");
        exit(1);
    }
    char* buffer = (char *)malloc(sizeof(char) * N);
    if(buffer == NULL) {
        perror("Buffer allocation error");
        exit(1);
    }
    int chars_read;
    while ((chars_read = fread(buffer,sizeof(char),N,supply)) == N) {
        /*if(chars_read != N) {
            for(int b = chars_read; b < N; b++){
                buffer[b] = ' ';
            }
        }*/
        sub_newlines(buffer);
        flock(fileno(production_output), LOCK_EX);
        fwrite(&row_number, sizeof(int),1,production_output);
        fwrite(buffer, sizeof(char),N,production_output);
        flock(fileno(production_output), LOCK_UN);
    }
    free(buffer);
    fclose(production_output);
    fclose(supply);
}
int main(int argc, char** argv) {
    switch (argc)
    {
    case 5:;
        if(!isnumber(argv[2]) || !isnumber(argv[4])) {
            puts("Row number and N - number of char read must be a postivie int");
            return -1;
        }
        
        produce(atoi(argv[2]), argv[3], atoi(argv[4]), argv[1]);
        break;
    
    default:
        puts("Wrong arg number (4 are requierd)");
        return -1;
    }
}