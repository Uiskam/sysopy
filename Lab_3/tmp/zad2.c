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

#define NULL_PID -1

/*
 * INPUT: 2 parametry:
 *      dok - dokładnośc obliczeń (liczba prostokątów na jakie dziele przedział)
 *      n - liczba procesów na jakie dzieli się obliczenia (procesy sa numerowne w kolejności ch tworzenia od 1)
 *
 * Procesy zapisują swoje wyniki do plików wN.txt (N to nadany nr procesu)
 * Proces macierzysty czeka aż wszyscy skończą i sumuje wszystko.
 *
 * Wynik obliczeń powinien zostac przedstawony na standardowym wyjściu.
 * Program ma również tworzyć raport dot. czasu wykonania obliczęń w zależności od dokładności i liczby procesów.
 * (Dokładność obliczeń ma byc taka, żeby obliczneia trwały min kilka sekund)
 *
 * Pradiwłowy wynik całki: pi = 3.14159265
 */

void wyp(int tab[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", tab[i]);
    printf("\n");
}

void clean_prev_files(int n) {
    char command[60];
    for (int i = 0; i < n; i++) {
        snprintf(command, 10, "rm w%d.txt", i);
        system(command);
    }
}

int find_relative_pid(int *tab, int size) {
    int rpid = 0;
    while (rpid < size && tab[rpid] != NULL_PID)
        rpid++;
    return rpid - 1;
}

double f(double x) {
    return (double) (4.0 / (x * x + 1));
}

void calculations(int pid, int square_qunatity, double square_width) {
    double begin = pid * square_qunatity * square_width;

    double result = 0;
    for (int i = 0; i < square_qunatity; i++) {
        result += f(begin) * square_width;
        begin += square_width;
    }
    FILE *output;
    char file_name[50];
    snprintf(file_name, 50, "w%d.txt", pid);
    output = fopen(file_name, "w");
    fprintf(output, "%f", result);
    fclose(output);
}

void result(int pid_count) {
    double result = 0;
    char file_name[50], partial_result[30];
    FILE *result_file;
    for (int cur_pid = 0; cur_pid < pid_count; cur_pid++) {
        snprintf(file_name, 50, "w%d.txt", cur_pid);
        result_file = fopen(file_name, "r");
        fscanf(result_file, "%s", partial_result);
        result += atof(partial_result);
    }
    printf("integral calculation result %lf\n", result);
}

double timeDifference(clock_t t1, clock_t t2){
    return ((double)(t2 - t1) / sysconf(_SC_CLK_TCK));
}

void writeResult(clock_t start, clock_t end, struct tms* t_start, struct tms* t_end, FILE* report_file){
    printf("\tREAL_TIME: %fl\n", timeDifference(start,end));
    //printf("\tUSER_TIME: %fl\n", timeDifference(t_start->tms_utime, t_end->tms_utime));
    //printf("\tSYSTEM_TIME: %fl\n", timeDifference(t_start->tms_stime, t_end->tms_stime));

    fprintf(report_file, "\tREAL_TIME: %fl\n", timeDifference(start, end));
    //fprintf(report_file, "\tUSER_TIME: %fl\n", timeDifference(t_start->tms_utime, t_end->tms_utime));
    //fprintf(report_file, "\tSYSTEM_TIME: %fl\n", timeDifference(t_start->tms_stime, t_end->tms_stime));
    fprintf(report_file,"\n");
}

int main(int argc, char **argv) {

    //clean_prev_files(20);
    if (argc != 3) {
        printf("Precisely two int arg are required!\n");
        return -1;
    }
    int n = atoi(argv[2]), square_quantity;
    double precision = atof(argv[1]);
    if (n <= 0 || precision <= 0) {
        printf("Postive double int are required as arg!\n");
        return -1;
    }
    clock_t start,stop;
    struct tms* time_start;
    struct tms* time_stop;
    time_start = calloc(1, sizeof (struct  tms));
    time_stop = calloc(1, sizeof (struct  tms));
    start = times(time_start);
    printf("%0.9lf %d\n", precision, n);

    square_quantity = 1 / precision;

    int i_am_mother = 10;
    for (int i = 0; i < n; i++) {
        i_am_mother = fork();
        if (i_am_mother == 0) {
            calculations(i, square_quantity / n, precision);
            i = n;
            break;
        }
    }

    if (i_am_mother != 0) {
        for (int i = 0; i < n; i++) {
            if (wait(NULL) == -1)
                break;
        }
        result(n);
        stop = times(time_stop);
        FILE *report_file;
        report_file = fopen("raport2.txt", "a");
        fprintf(report_file, "Dokładność %0.9lf liczba procesów %d \n", precision, n);
        writeResult(start,stop,time_start,time_stop, report_file);
        fclose(report_file);


    }
}