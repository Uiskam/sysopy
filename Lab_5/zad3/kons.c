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
int production_input_type_check(char* filepath) {
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

void consume(char* input_path, char* output_path, int N) {
    FILE* input = fopen(input_path, "r");
    if(input == NULL) {
        perror("Pipe open fail!");
        exit(1);
    }
    if(production_input_type_check(input_path) == 0){
        puts("Wrong file type of file given");
        exit(1);
    }
    
    int row_number;
    char* product = (char *)malloc(sizeof(char) * (N + 1));
    if(product == NULL){
        perror("Memomry alloaction error");
        exit(1);
    }
    FILE* output = fopen(output_path,"w+");
        if(output == NULL) {
            perror("openieng out file error");
            exit(1);
        }   
    while (fread(&row_number, sizeof(int), 1, input)){
        
        flock(fileno(output), LOCK_EX);
        fread(product, sizeof(char), N, input);
        int cur_row = 0;
        char c;
        fseek(output, 0, SEEK_SET);
        while (fread(&c, sizeof(char), 1 , output)) {
            if(c == '\n') {
                cur_row++;
            }
            if(cur_row >= row_number) {
                fseek(output, -1, SEEK_CUR);
                char* rest = (char *)malloc(4096 * sizeof(char));
                int rest_size = fread(rest, sizeof(char), 4096, output);
                if(rest == NULL) exit(1);
                fseek(output, -rest_size, SEEK_CUR);
                fwrite(product, sizeof(char), N, output);
                fwrite(rest, sizeof(char), rest_size, output);
                free(rest);
                break;
            }
        }
        if(cur_row < row_number) {
            for(int i = cur_row + 1; i <= row_number; i++) {
                char new_row[2] = "\n";
                fwrite(new_row, sizeof(char), strlen(new_row), output);
            }
            fseek(output, -1, SEEK_CUR);
            fwrite(product, sizeof(char), N, output);
            fwrite("\n", sizeof(char), 1, output);
        }
        flock(fileno(output), LOCK_UN);
    }
    fclose(output);
    fclose(input);
    free(product);
    
}
int main(int argc, char** argv) {
    switch (argc)
    {
    case 4:
        if(!isnumber(argv[3]))  {
            puts("N - number of char read must be a postivie int");
            return -1;
        }
        consume(argv[1], argv[2], atoi(argv[3]));
        break;
    
    default:
        puts("Wrong arg number (3 are requierd)");
        return -1;
    }
}