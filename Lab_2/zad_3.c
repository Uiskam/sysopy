#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <ftw.h>

int reg_file = 0;
int dir_file = 0;
int chr_file = 0;
int blk_file = 0;
int fifo_file = 0;
int symbolic_file = 0;
int socket_file = 0;

void printFileProperties(struct stat stats, char file_name[]);

void searchThrough(char dir_name[]);

int main(int argc, char **argv) {
    if(argc != 2) {
        printf("WRONG ARG NUMBER!\n");
        return 0;
    }
    struct stat first;
    stat(argv[1], &first);
    if(S_ISLNK(first.st_mode)){
        printf("Arg must be a directory (not slink)!\n");
        return 0;
    }

    searchThrough(argv[1]);
    printf("Liczba:\nkatalgow: %d\nregularanych: %d\nznakowych: %d\nblkowowe: %d\npotoki: %d\ndowiazania_sumboliczne: %d\nsockety: %d\n",
           dir_file, reg_file, chr_file, blk_file, fifo_file, symbolic_file, socket_file);
    return 0;
}

void searchThrough(char dir_name[]) {
    struct dirent *de;
    DIR *dr = opendir(dir_name);
    if (dr == NULL) {
        printf("Could not open current directory");
        return;
    }
    struct stat stats;
    while ((de = readdir(dr)) != NULL) {
        printf("Sciezka bezewzlgedna: %s \n", realpath(de->d_name, NULL));
        if (stat(de->d_name, &stats) == 0) {
            printFileProperties(stats,de->d_name);
        }
        printf("\n");
    }
    closedir(dr);
}

void printFileProperties(struct stat stats, char file_name[]) {
    bool czy = false;
    struct tm dt;
    printf("Liczba dowiazan: %d\n", stats.st_nlink);
    printf("Typ pliku: ");
    if (S_ISREG(stats.st_mode)) {
        printf("regularny\n");
        reg_file++;
    } else if (S_ISDIR(stats.st_mode)) {
        printf("katalog\n");
        dir_file++;
        if(strcmp(file_name,".") && strcmp(file_name,"..")){
            czy = true;
        }
    } else if (S_ISCHR(stats.st_mode)) {
        printf("urzadzenia znakowe\n");
        chr_file++;
    } else if (S_ISBLK(stats.st_mode)) {
        printf("urzadznie blowkowe\n");
        blk_file++;
    } else if (S_ISFIFO(stats.st_mode)) {
        printf("potok\n");
        fifo_file++;
    } else if (S_ISLNK(stats.st_mode)) {
        printf("link symboliczny\n");
        symbolic_file++;
    } else if (S_ISSOCK(stats.st_mode)) {
        printf("socekt\n");
        socket_file++;
    }
    printf("Rozmiar w bajtach: %d\n", stats.st_size);
    dt = *(gmtime(&stats.st_atime));
    printf("Czas ostatniego dostepu: %d-%d-%d\n", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900);
    dt = *(gmtime(&stats.st_mtime));
    printf("Ostatnio modyfikowano: %d-%d-%d\n", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900);
    if(czy){
        searchThrough(file_name);
    }
}