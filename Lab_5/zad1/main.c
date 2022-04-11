#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        puts("invalid args number, correct [filepath]");
        return -1;
    }
    FILE *command_file = fopen(argv[1], "r");
    if (command_file == NULL) {
        puts("wrong command file: %s is non existing", argv[1]);
        return -1;
    }


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