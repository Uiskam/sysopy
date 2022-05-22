#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* image_file;
FILE* result_file;
int check_args_validity(int argc, char** argv) {
    if(argc != 5) {
        puts("Wrong arg number! There must be 4 args:\n\tthread number(positive int),\n\t\"number\"/\"block\" mode selector,\n\t"
             "input picture file name,\n\tname of a result file.");
        return -1;
    }
    if(atoi(argv[1]) <= 0) {
        printf("Thread number must be positive int, %s was passed\n",argv[1]);
        return -1;
    }
    if(strcmp(argv[2],"numbers") && strcmp(argv[2],"block")) {
        printf("Mode must be \"numbers\" or \"block\", %s was passed\n",argv[2]);
        return -1;
    }
    if((image_file = fopen(argv[3],"r")) == NULL) {
        perror("Image file opening error");
        return -1;
    }
    if((result_file = fopen(argv[4],"w")) == NULL) {
        perror("Result file creation error");
        return -1;
    }
    return 0;
}

int main(int argc, char** argv) {
    if(check_args_validity(argc, argv) == -1) return -1;

}