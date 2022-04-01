#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ftw.h>

int reg_file = 0;
int dir_file = 0;
int symbolic_file = 0;

char *type_of_file_nftw(int type){
    switch(type) {
        case FTW_F:
            reg_file++;
            return "file";
        case FTW_D:
            dir_file++;
            return "dir";
        case FTW_SL:
            symbolic_file++;
            return "link";
        default: return "";
    };
}

int display_info(const char *path, const struct stat *sb, int type_of_flag, struct FTW *tw_buf){
    struct tm *modTime;
    struct tm *accTime;
    char buffer[50];
    char *absPath = (char*) calloc(100, sizeof(char));

    modTime = localtime((const time_t *) &sb->st_mtime);
    accTime = localtime((const time_t *) &sb->st_atime);

    realpath(path, absPath);

    printf("\nSciezka bezwzgledna: %s\n"
           "Typ pliku: %s\n"
           "Rozmwiar w bajtach %ld\n",
           absPath,
           type_of_file_nftw(type_of_flag),
           sb->st_size);
    strftime (buffer, 50 ,"Ostatnio modyfikowano: %d.%m.%Y\n", modTime);
    printf("%s", buffer);
    strftime (buffer, 50 ,"Czas ostatniego dostepu: %d.%m.%Y\n", accTime);
    printf("%s", buffer);
    free(absPath);
    return 0;
}

void _nftw(char *dir){
    nftw(dir, display_info, 100, FTW_PHYS);
}


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
    _nftw(argv[1]);
    printf("Liczba:\nkatalgow: %d\nregularanych: %d\nznakowych: %d\nblkowowe: %d\npotoki: %d\ndowiazania_sumboliczne: %d\nsockety: %d\n",
           dir_file, reg_file, symbolic_file);
    return 0;
}