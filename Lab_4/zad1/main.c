#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>


/*
 * args: ignore || handler || mask || pending
 * działanie, odpowiednio: w przdoku ustawia:
 *  - ignorowanie
 *  - instalacja handlera, który wypisuje komunikat o otrzymaniu sygnału
 *  - maskuje sygnał
 *  - sprawdza, czy ozekujący sygnał jest widoczny w procesie
 *
 *  Za pomoca raise wysyła sygnał do samego siebie (chyba że jest pending, wtedy sprawdza czy sygnał oczekujacy w przedku jest w potomku)
 *
 *  Zrobić to dla fork i exec, sprawdzić wyniki i wnioski zapisać w raport2.txt
 *  (w instrukcji jest że exec dziedziczy sygnały a fork nie)
 */

void handler(int sig) {
    printf("Handler - Signal received!\n");
}

int main(int argc, char **argv) {
    if(argc != 3) {
        perror("Wrong arg number!\n");
        return -1;
    }
    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0) {
        sigset_t new_mask; //zbiór sygnałów, umożliwa grupowanie ich aby można było zarządzac cała ich grupą
        sigemptyset(&new_mask); //inicjalizacja pustego zbioru
        sigaddset(&new_mask, SIGUSR1); //dodanie sygnału edo maski
        if (sigprocmask(SIG_BLOCK, &new_mask, NULL) < 0) //ustawia maskę dla aktualnego procesu return 0 if setting was succesfull -1 in other case
            perror("Signal was not blocked\n");
    } else if (strcmp(argv[1], "handler") == 0) {
        signal(SIGUSR1, handler); //ustawia funkcje do obslugi sygnaly (funckja ta musi byc void i jako arg musi miec nr sygnalu do przechycebia
    } else if (strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);//SIG_IGN to oznaczenie że sygnał ma być ignorowany
    }
    raise(SIGUSR1);// równoważne z kill(getpid(), sig);
    sigset_t new_mask;

    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0) {
        sigpending(&new_mask); //odczytuje liuste sygnałów oczeukjącuch
        printf("Signal pending: %d\n", sigismember(&new_mask, SIGUSR1));//sigmemeber == contain?
    }

    if (strcmp(argv[2], "exec") == 0) {
        execl("./exec", "./exec", argv[1], NULL);
    } else {
        pid_t child_pid = fork();
        if (child_pid == 0) {
            if (strcmp(argv[1], "pending") != 0) {
                raise(SIGUSR1);
            }
            if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0) {
                sigpending(&new_mask);
                printf("Signal pending: %d\n", sigismember(&new_mask, SIGUSR1));
            }
        }
    }
    return 0;
}