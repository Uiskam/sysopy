#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_FILE_NAME_SIZE 100
#define BUFFER_SIZE 10000
FILE *file_input;
FILE *file_output;
FILE *time_file;
bool white_char(char ch) {
    if (ch != 32 && ch != 9 && ch != 11 && ch != 12 && ch != 13 && ch != 10)
        return false;
    return true;
}

int lib_c_read_and_write() {
    char buffer[BUFFER_SIZE];
    char buffer_cleared[BUFFER_SIZE];
    int chars_red = fread(buffer, sizeof(char), BUFFER_SIZE, file_input);
    int clean_index = 0;
    for (int i = 0; i < chars_red; i++, clean_index++) {
        buffer_cleared[clean_index] = buffer[i];
        if (buffer[i] == 10) {
            for (int j = i; j < chars_red && white_char(buffer[j]); j++) {
                i++;
            }
            i--;
        }
    }
    int chars_written = fwrite(buffer_cleared, sizeof(char), clean_index, file_output);
    if (chars_written != clean_index) {
        printf("WRITING ERROR\n");
        exit(1);
    }
    return chars_red;
}

void lib_c_implementation(char file_name[]) {
    file_input = fopen(file_name, "r");
    if (file_input == NULL) {
        printf("File %s do not exists\n", file_name);
        return;
    }
    file_output = fopen(strcat(file_name, ".copy"), "w");
    int chars_red;
    do {
        chars_red = lib_c_read_and_write();
    } while (chars_red == BUFFER_SIZE);
    fclose(file_input);
    fclose(file_output);
}

int sys_read_and_write(int fd, int out_fd) {
    char buffer[BUFFER_SIZE];
    char buffer_cleared[BUFFER_SIZE];
    int chars_red = read(fd, buffer, BUFFER_SIZE);
    int clean_index = 0;
    for (int i = 0; i < chars_red; i++, clean_index++) {
        buffer_cleared[clean_index] = buffer[i];
        if (buffer[i] == 10) {
            for (int j = i; j < chars_red && white_char(buffer[j]); j++) {
                i++;
            }
            i--;
        }
    }
    int chars_written = write(out_fd, buffer_cleared, clean_index);
    return chars_red;
}

void sys_implementation(char file_name[]) {
    int fd = open(file_name, O_RDONLY);

    if (fd == -1) {
        printf("File %s do not exists\n", file_name);
        return;
    }
    char new_file_name[MAX_FILE_NAME_SIZE];
    strcpy(new_file_name, file_name);
    strcat(new_file_name, ".copy");
    int out_fd = open(new_file_name, O_WRONLY | O_CREAT);
    int chars_red;
    do {
        chars_red = sys_read_and_write(fd, out_fd);
    } while (chars_red == BUFFER_SIZE);
    close(fd);
    close(out_fd);
}

void writeResult(double time, char msg[]) {
    printf( "%s %fl\n",msg, time);
    fprintf(time_file, "%s %fl\n",msg, time);
}

int main(int argc, char **argv) {
    clock_t start, end;
    char time_file_name[] = "pomiar_zad_1.txt";
    time_file = fopen(time_file_name,"w");
    if (argc == 1) {
        char file_name[MAX_FILE_NAME_SIZE];
        int files_count;
        printf("Podaj liczbe plików: ");
        scanf("%d", &files_count);
        for (int i = 0; i < files_count; i++) {
            printf("Nazwa pliku nr %d (nazwa max 100 znakow): ", i);
            scanf("%s", file_name);
            start = clock();
            sys_implementation(file_name);
            end = clock();
            writeResult((double)(end - start)/ CLOCKS_PER_SEC, "SYS elapsed time: ");
            start = clock();
            lib_c_implementation(file_name);
            end = clock();
            writeResult((double )(end - start)/CLOCKS_PER_SEC, "LIB elapsed time: ");
        }
    }
    for (int i = 1; i < argc; i++) {
        start = clock();
        sys_implementation(argv[i]);
        end = clock();
        writeResult((double)(end - start)/ CLOCKS_PER_SEC, "SYS elapsed time: ");
        start = clock();
        lib_c_implementation(argv[i]);
        end = clock();
        writeResult((double )(end - start)/CLOCKS_PER_SEC, "LIB elapsed time: ");
    }
    fclose(time_file);
}