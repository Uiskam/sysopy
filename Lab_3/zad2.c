#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
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
    for(int i = 0; i < size; i++)
        printf("%d ", tab[i]);
    printf("\n");
}

void clean_prev_files(int n) {
    char command[60];
    for(int i = 0; i < n; i++) {
        snprintf(command,10,"rm w%d.txt",i);    
        //strcat(command, " > /dev/null");printf("%s command\n", command);
        system(command);
    }
}
int find_relative_pid(int* tab, int size) {
    int rpid = 0;
    while (rpid < size && tab[rpid] != NULL_PID)
        rpid++;
    return rpid - 1;
}
double f(double x) {
    return (double)(4.0 / (x * x + 1));
}
void calculations(int pid, int square_qunatity, double square_width) {
    double begin = pid * square_qunatity * square_width;
    printf("begin %f pid %d\n",begin,pid);
    double result = 0;
    for(int i = 0; i < square_qunatity; i++) {
        result += f(begin) * square_width;
        if(pid == 0)
        begin += square_width;
    }
    
    FILE* output;
    char file_name[50];
    snprintf(file_name,50,"w%d.txt",pid);    
    output = fopen(file_name, "w");
    fprintf(output,"%f",result);
    //printf("res of %d: %f file name: %s\n", pid, result, file_name);
    fclose(output);
}

void result(int pid_count) {
    double result = 0;
    char file_name[50], partial_result[30];
    FILE* result_file;
    for(int cur_pid = 0 ; cur_pid < pid_count; cur_pid++) {
        snprintf(file_name,50,"w%d.txt",cur_pid);
        result_file = fopen(file_name, "r");
        fscanf(result_file, "%s", partial_result);
        result += atof(partial_result);
    }
    printf("integral calculation result %f\n", result);
}

int main(int argc, char **argv) {
    //clean_prev_files(20);
    if (argc != 3) {
        printf("Precisely two int arg are required!\n");
        return -1;
    }
    int n = atoi(argv[2]), square_quantity, *process_register = (int*)malloc(n * sizeof(int));;
    double precision = atof(argv[1]);
    precision = 0.000000001;
    if (n <= 0 || precision <= 0) {
        printf("Postive double int are required as arg!\n");
        return -1;
    }

    printf("%f %d\n",precision, n);
    square_quantity= 1 / precision;
    for(int i = 0; i < n; i++) 
        process_register[i] = NULL_PID;

    
    int i_am_mother = 10;
    for (int i = 0; i < n; i++) {
        process_register[i] = i;
        //printf("new proc %d %d %d\n",i,n,i_am_mother);
        i_am_mother = fork();
        if(i_am_mother == 0) {
            
            calculations(i, square_quantity / n, precision);
            i = n;
            break;
        }
    }

    if(i_am_mother != 0){
        for(int i = 0; i < n; i++){
            if (wait(NULL) == -1)
                break;
        }
        result(n);
    }
}