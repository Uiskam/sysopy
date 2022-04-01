#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

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
 */

int find_relative_pid(int* tab, int size) {
    int rpid = 0;
    while (rpid < size && tab[rpid] != 0)
        rpid++;
    return rpid;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Precisely two int arg are required!\n");
        return -1;
    }
    int n = atoi(argv[2]), precision = atoi(argv[1]);
    int *process_register = (int*)calloc(n,sizeof(int));
    if (n <= 0 || precision <= 0) {
        printf("Postive int is required as arg!\n");
        return -1;
    }
    int i_am_mother = 0;
    for (int i = 0; i < n; i++) {
        process_register[i] = i+1;
        i_am_mother = fork();
        if(i_am_mother == 0) {
            printf("My PID is %d\n", i + 1);
            wyp(process_register,n);
            i = n;
        }
    }

    if(i_am_mother != 0){
        for(int i = 0; i < n; i++){
            if (wait(NULL) == -1)
                break;
        }
    }
}