#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#define MAX_FILE_NAME_SIZE 100
#define BUFFER_SIZE 10000
#define MODE 1
FILE *file_input;
FILE *file_output;
FILE *time_file;
int chr_counter = 0;
int line_counter = 0;
bool white_char(char ch) {
    if (ch != 32 && ch != 9 && ch != 11 && ch != 12 && ch != 13 && ch != 10)
        return false;
    return true;
}
void writeResult(double time, char msg[]) {
    printf( "%s %fl\n",msg, time);
    fprintf(time_file, "%s %fl\n",msg, time);
}
int lib_c_read_and_write(char chr) {
    char buffer[BUFFER_SIZE];
    int chars_red = fread(buffer, sizeof(char), BUFFER_SIZE, file_input);
    bool new_line = true;
    for(int i = 0; i < chars_red; i++) {
        if(buffer[i] == chr) {
            chr_counter++;
            if(new_line){
                line_counter++;
                new_line = false;
            }
        }
        if(buffer[i] == 10)
            new_line = true;
    }
    return chars_red;
}

void lib_c_implementation(char chr, char file_name[]) {
    file_input = fopen(file_name, "r");
    if(file_input == NULL){
        printf("File %s do not exists\n", file_name);
        return;
    }
    int chars_red;
    do {
        chars_red = lib_c_read_and_write(chr);
    } while (chars_red == BUFFER_SIZE);
    fclose(file_input);
}

int sys_read_and_write(int fd, char chr) {
    char buffer[BUFFER_SIZE];
    int chars_red = read(fd, buffer, BUFFER_SIZE);
    bool new_line = true;
    for(int i = 0; i < chars_red; i++) {
        if(buffer[i] == chr) {
            chr_counter++;
            if(new_line){
                line_counter++;
                new_line = false;
            }
        }
        if(buffer[i] == 10)
            new_line = true;
    }
    return chars_red;
}

void sys_implementation(char chr, char file_name[]) {
    int fd = open(file_name, O_RDONLY);
    if(fd == -1) {
        printf("File %s do not exists\n", file_name);
        return;
    }
    int chars_red;
    do {
        chars_red = sys_read_and_write(fd, chr);
    } while (chars_red == BUFFER_SIZE);
    close(fd);
}

int main(int argc, char **argv) {
    char time_file_name[] = "pomiar_zad_2.txt";
    time_file = fopen(time_file_name,"w");
    clock_t start, end;
    if(argc != 3){
        printf("Wrong arguments quanitity (2 are required)\n");
        exit(-1);
    }
    if(strlen(argv[1]) != 1){
        printf("First argument should be one char\n");
        exit(-1);
    }
    chr_counter = 0;
    line_counter = 0;
    start = clock();
    sys_implementation(argv[1][0], argv[2]);
    end = clock();
    writeResult((double)(end - start)/ CLOCKS_PER_SEC, "SYS elapsed time: ");
    printf("SYS\nLiczba wystapien znaku %c w pliku %s: %d\nLiczba wierszy z tym znakiem: %d\n",argv[1][0], argv[2], chr_counter, line_counter);

    chr_counter = 0;
    line_counter = 0;
    start = clock();
    lib_c_implementation(argv[1][0], argv[2]);
    end = clock();
    writeResult((double )(end - start)/CLOCKS_PER_SEC, "LIB elapsed time: ");
    printf("LIB\nLiczba wystapien znaku %c w pliku %s: %d\nLiczba wierszy z tym znakiem: %d\n",argv[1][0], argv[2], chr_counter, line_counter);

}