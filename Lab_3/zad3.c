#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/*
argumenty: katalog wzorzec_szukany max_głebokość_szukania
działanie: 
- prgoram przeszuje drzewo katalogow w poszukiwaniu plikow (TEKSTOWYCH) zawierających wzorzec_szukany
- podkatalogi przeszukujemy w procesach potomonych
- wypis: ściezka wzgledna do katalogu(jako poczatek traktowany jest arg), PID, zawa pliku
*/
/*bool search_for_text(FILE* file, char* text_to_find) {
    ;//system("grep %s ",text_to_find)
}*/
bool is_dir(const char *path) {
    struct stat file_stat;
    stat(path, &file_stat);
    return S_ISDIR(file_stat.st_mode);
}

bool is_text_file(const char *path) {
    char command[200];
    snprintf(command, 200, "file %s | grep text > /dev/null", path);
    int res = system(command);
    if (res == 0)
        return true;
    return false;
}

void search_through(DIR *folder, int cur_depth, int max_depth) {
    struct dirent *folder_content;
    while ((folder_content = readdir(folder))) {
        if (is_dir(folder_content->d_name) &&
            strcmp(folder_content->d_name, ".") != 0 && strcmp(folder_content->d_name, "..") != 0 && cur_depth < max_depth) {
            printf("cur %d max %d dir %s\n",cur_depth,max_depth, folder_content->d_name);
        }
        if (is_text_file(folder_content->d_name)) {
            printf("text file %s\n", folder_content->d_name);
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Wrong args number! Program accepts 4 args:\n\t "
               "(dir_name, text_to_find, searching_depth (1 is searching only through dir_name))\n");
        return -1;
    }
    int search_depth = atoi(argv[3]);
    if (search_depth == 0) {
        printf("search_depth must be at least 1!\n");
        return -1;
    }
    DIR *main_folder = opendir(argv[1]);
    if (main_folder == NULL) {
        printf("Given dir do not exist!\n");
        return -1;
    }
    search_through(main_folder, 1, search_depth);
}