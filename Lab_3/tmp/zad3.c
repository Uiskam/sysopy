#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/wait.h>

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
//DIR *main_folder;
char* main_folder_name;
bool is_dir(const char *path) {
    struct stat file_stat;
    stat(path, &file_stat);
    return S_ISDIR(file_stat.st_mode);
}

bool is_text_file(const char *path) {
    char command[200];
    snprintf(command, 200, "file \"%s\" | grep text > /dev/null", path);
    int res = system(command);
    if (res == 0)
        return true;
    return false;
}

bool contains_text(const char *path, const char *text) {
    char command[500];
    snprintf(command, 500, "grep \"%s\" \"%s\" > /dev/null", text, path);
    int res = system(command);
    if (res == 0)
        return true;
    return false;
}
int main_pid;

void search_through(DIR *folder, const int cur_depth, const int max_depth, const char* text, char *cur_path) {
    struct dirent *folder_content;
    while ((folder_content = readdir(folder))) {
        if(strcmp(folder_content->d_name, ".") == 0 || strcmp(folder_content->d_name, "..") == 0)
            continue;

        char file_path[300];
        strcpy(file_path, cur_path);
        strcat(file_path, folder_content->d_name);

        if (is_dir(file_path) && cur_depth < max_depth) {
            int pid = fork();
            if(pid == -1) {
                printf("sth went wrong with fork()\n");
                return;
            } else if(pid == 0) {
                DIR *sub_folder;
                sub_folder = opendir(file_path);
                if(sub_folder == NULL){
                    return;
                }
                char new_path[200];
                strcpy(new_path, cur_path);
                //strcat(new_path, "/");
                strcat(new_path, folder_content->d_name);
                strcat(new_path, "/");
                search_through(sub_folder, cur_depth + 1, max_depth, text,new_path);
                closedir(sub_folder);
                return;
            }

        }
        //if(getpid() > main_pid)printf("pid: %d file_name %s real \n%d\n\n", getpid(), file_path,is_text_file(file_path));

        if (is_text_file(file_path)) {
            if(contains_text(realpath(file_path, NULL),text)) {
                printf("relative path: %s\nprocess PID: %d\nfile name: %s\n\n", file_path/*realpath(folder_content->d_name, NULL)*/, getpid(),folder_content->d_name);
            }
        }

    }

    closedir(folder);
    return;
}

int main(int argc, char **argv) {
    main_pid = getpid();
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
    DIR* main_folder = opendir(argv[1]);
    if (main_folder == NULL) {
        printf("Given dir do not exist!\n");
        return -1;
    }
    basename(realpath(argv[1], main_folder_name));
    char path[100];
    strcpy(path, argv[1]);
    strcat(path, "/");
    search_through(main_folder, 1, search_depth, argv[2], path);
    wait(NULL);
    closedir(main_folder);
    free(main_folder_name);
}
