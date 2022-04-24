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
    FILE* output = fopen(output_path,"w");
    if(input == NULL) {
        perror("Pipe open fail!");
        exit(1);
    }
    if(production_input_type_check(input_path) == 0){
        puts("Wrong file type of file given");
        exit(1);
    }

    
}
int main(int argc, char** argv) {
    FILE* tmp = fopen(argv[1], "r");
    if(tmp == NULL) {
        return -1;
    }
    int read;
    //int N = atoi(argv[2]);
    char* buffer = (char *)malloc(100*sizeof(char));
    //printf("A\n");
    while ((read = fread(buffer,1 , sizeof(char),tmp)))
    {
        //printf("XDDDD\n");
        printf("%s",buffer);
    }
    //printf("A\n");
    free(buffer);
    fclose(tmp);
    return 0;
    switch (argc)
    {
    case 4:
        if(!isnumber(argv[3]))  {
            puts("N - number of char read must be a postivie int");
            return -1;
        }

        break;
    
    default:
        puts("Wrong arg number (3 are requierd)");
        return -1;
    }
}