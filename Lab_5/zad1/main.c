#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*
    argument:
        - sciezka do pliku do interpretacji
    działanbie:
        zinterpetowanie pliku i odpalenie progamów połozocznych pipami kazdy w osobnym procsei za pomoca exec
*/
#define MAX_SKLADNIK_SIZE 5
#define MAX_ARG_NUMBER 10
#define MAX_SKLADNIK_NUMBER 10
void wyp(char tab[MAX_SKLADNIK_NUMBER][MAX_SKLADNIK_SIZE][MAX_ARG_NUMBER]) {
    printf("XD");
}
int main(int argc, char **argv) {
    
    if (argc != 2) {
        puts("invalid args number, correct [filepath]");
        return -1;
    }
    FILE *command_file = fopen(argv[1], "r");
    if (command_file == NULL) {
        printf("wrong command file: %s is non existing\n", argv[1]);
        return -1;
    }
    char components[MAX_SKLADNIK_NUMBER][MAX_SKLADNIK_SIZE][MAX_ARG_NUMBER];
    
    ssize_t nread;
    size_t len;
    char* line = NULL;
    char* after_eq;
    char letter;
    int cur_compoment = 0;
    while ((nread = getline(&line, &len, command_file)) != -1) {
        int cur_command = 0;
        after_eq = strchr(line,'=');
        char arg[1000] = "";
        for(int i = after_eq - line + 1; i < len; i++) {
            letter = line[i];
            if(letter == '|'){cur_command++ ;continue;}
            if(letter == ' ') {
                printf("arg: %s\n",arg);
                strcpy(arg,"");
                while (letter == ' '){
                    i++;
                    letter = line[i];
                }
            }
            if(letter == '\n') {
                printf("arg: %s\n", arg);
            }
            strcat(arg,&letter);
            
        }
        puts("");

        
    }
    fclose(command_file);
    free(line);
       
}


/*
 * if(argc != 2){
        puts("invalid args number");
        return -1;
    }
    FILE* command_file = fopen(argv[1],"r");
    if(command_file == NULL) {
        puts("wrong command file non existing");
        return -1;
    }
 */