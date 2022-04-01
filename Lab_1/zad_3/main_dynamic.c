#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include "mylib.h"
#include <time.h>
#include <sys/times.h>
#include <dlfcn.h>
#include <unistd.h>


FILE *reportFile;

void print_error(char *msg) {
    printf("%s", msg);
    exit(1);
}

double timeDifference(clock_t t1, clock_t t2) {
    return ((double) (t2 - t1) / sysconf(_SC_CLK_TCK));
}

void writeResult(clock_t start, clock_t end, struct tms *t_start, struct tms *t_end) {
    printf("\tREAL_TIME: %fl\n", timeDifference(start, end));
    printf("\tUSER_TIME: %fl\n", timeDifference(t_start->tms_utime, t_end->tms_utime));
    printf("\tSYSTEM_TIME: %fl\n", timeDifference(t_start->tms_stime, t_end->tms_stime));

    fprintf(reportFile, "\tREAL_TIME: %fl\n", timeDifference(start, end));
    fprintf(reportFile, "\tUSER_TIME: %fl\n", timeDifference(t_start->tms_utime, t_end->tms_utime));
    fprintf(reportFile, "\tSYSTEM_TIME: %fl\n", timeDifference(t_start->tms_stime, t_end->tms_stime));
}

int main(int argc, char **argv) {
    void *handle = dlopen("./libmylib.so",RTLD_LAZY);
    if(!handle)
    {
        printf("BŁĄD!!!!!!!!!");
        exit(100);
    }
    int size = argc;
    reportFile = fopen("./raport3a.txt", "a");
    struct tms *tms[size];
    clock_t time[size];
    for (int i = 0; i < size; i++) {
        tms[i] = calloc(1, sizeof(struct tms *));
        time[i] = 0;
    }

    if (argc < 2) {
        print_error("Data input print_error");
        exit(1);
    }
    struct DataArray (*createDataArray)() = (struct DataArray (*)())dlsym(handle, "createDataArray");
    struct Block (*count_values)() = (struct Block(*)()) dlsym(handle,"count_values");
    int (*addToDataArray)() = (int (*)())dlsym(handle, "addToDataArray");
    void (*removeBlock)() = (void (*)())dlsym(handle, "removeBlock");
    int (*isNumber)() = (int (*)())dlsym(handle, "isNumber");

    int current = 0;
    struct Block tmp;
    struct DataArray myArray;
    for (int i = 2; i < argc; ++i) {
        time[current] = times(tms[current]);
        current += 1;

        if (strcmp(argv[i], "create_table") == 0) {
            if (!isNumber(argv[i + 1])) {
                print_error("Invalid input data");
                exit(1);
            }
            printf("Created table with size %s:\n", argv[i + 1]);
            fprintf(reportFile, "Created table with size %s:\n", argv[i + 1]);
            myArray = (*createDataArray)(atoi(argv[i + 1]));
            i += 1;
        } else if (strcmp(argv[i], "wc_files") == 0) {
            int how_many = 0;
            for (int j = i + 1; j < argc; j++) {
                if (strcmp(argv[j], "create_table") != 0 && strcmp(argv[j], "wc_files") != 0 &&
                    strcmp(argv[j], "remove_block") != 0) {
                    how_many++;
                } else {
                    break;
                }
            }
            for (int j = 1; j <= how_many; j++) {
                tmp = count_values(argv[i + j]);
                printf("%s opened, results are: %d lines, %d words, %d characters\n", argv[i + j], tmp.line_count,
                       tmp.word_count, tmp.char_count);
                fprintf(reportFile, "%s opened, results are: %d lines, %d words, %d characters\n", argv[i + j],
                        tmp.line_count,
                        tmp.word_count, tmp.char_count);
                myArray.index = (*addToDataArray)(myArray, tmp);
            }
            i += how_many;
        } else if (strcmp(argv[i], "remove_block") == 0) {
            if (!isNumber(argv[i + 1]))
            {
                print_error("Invalid input data2");
                exit(1);
            }
            printf("Removed block %s:\n", argv[i + 1]);
            fprintf(reportFile, "Removed block %s:\n", argv[i + 1]);
            (*removeBlock)(myArray, atoi(argv[i + 1]));
            i += 1;
        } else
        {
            print_error(argv[i]);
            exit(1);
        }
        time[current] = times(tms[current]);
        writeResult(time[current - 1], time[current],
                    tms[current - 1], tms[current]);
        current += 1;
    }

    return 0;
}

