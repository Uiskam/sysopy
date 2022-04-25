#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/file.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    FILE* input = fopen("pipe", "r");
    if(input == NULL) {
        perror("Pipe open fail!");
        exit(1);
    }
    /*for (int i = 0; false; i++) {
            if (wait(NULL) == -1)
                break;
    }*/
    int row_number;
    char buf[5000];
    puts("XD");
    printf("result:\n");
    while (fread(buf, sizeof(char), 5, input)){
    printf("%s",buf);
    }
}