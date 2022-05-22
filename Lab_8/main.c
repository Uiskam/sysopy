#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_IMAGE_WIDTH 1920
#define MAX_IMAGE_HEIGHT 1080

FILE *IMAGE_FILE = NULL;
FILE *RESULT_FILE = NULL;
int IMAGE_WIDTH;
int IMAGE_HEIGHT;
int MAX_VAL;

int close_files() {
    if (pclose(IMAGE_FILE) == -1) {
        perror("image file closing");
        return 0;
    }
    if (pclose(RESULT_FILE) == -1) {
        perror("result file closing");
        return 0;
    }
    return -1;
}

int check_args_validity(int argc, char **argv) {
    if (argc != 5) {
        puts("Wrong arg number! There must be 4 args:\n\tthread number(positive int),\n\t\"number\"/\"block\" mode selector,\n\t"
             "input picture file name,\n\tname of a result file.");
        for (int i = 0; i < argc; i++) {
            printf("arg no %d: %s\n", i, argv[1]);
        }
        return -1;
    }
    if (atoi(argv[1]) <= 0) {
        printf("Thread number must be positive int, %s was passed\n", argv[1]);
        return -1;
    }
    if (strcmp(argv[2], "numbers") && strcmp(argv[2], "block")) {
        printf("Mode must be \"numbers\" or \"block\", %s was passed\n", argv[2]);
        return -1;
    }
    if ((IMAGE_FILE = fopen(argv[3], "r")) == NULL) {
        perror("Image file opening error");
        return -1;
    }
    if ((RESULT_FILE = fopen(argv[4], "w")) == NULL) {
        perror("Result file creation error");
        return -1;
    }
    return 0;
}

int read_image_file(int image[MAX_IMAGE_WIDTH][MAX_IMAGE_HEIGHT]) {
    char *input_line = NULL;
    size_t len = 0;
    //ssize_t chars_read;

    getline(&input_line, &len, IMAGE_FILE);
    if (input_line[0] != 'P' && input_line[1] != '2') {
        printf("First line of .pgm file should be P2 not %s\n", input_line);
        free(input_line);
        return -1;
    }

    getline(&input_line, &len, IMAGE_FILE);
    char *token = strtok(input_line, " ");
    IMAGE_WIDTH = atoi(token);
    token = strtok(NULL, " ");
    IMAGE_HEIGHT = atoi(token);
    if (IMAGE_WIDTH <= 0 || IMAGE_HEIGHT <= 0) {
        printf("Wrong image size (width %d height %d)\n", IMAGE_WIDTH, IMAGE_HEIGHT);
        close_files();
        free(input_line);
        return -1;
    }

    getline(&input_line, &len, IMAGE_FILE);
    MAX_VAL = atoi(strtok(input_line, " "));
    if (MAX_VAL <= 0) {
        printf("Wrong max val: %d\n", MAX_VAL);
        free(input_line);
        return -1;
    }

    printf("img width %d img height %d \n", IMAGE_WIDTH, IMAGE_HEIGHT);
    int i = 0, j = 0, control_sum = 0, number;
    while (getline(&input_line, &len, IMAGE_FILE) != -1) {
        //printf("line read: %s", input_line);
        token = strtok(input_line, " ");
        number = atoi(token);
        while (token != NULL) {
            if (number > 0 || (number == 0 && token[0] == '0')) {
                if (number == 0) {
                    printf("0 number, token %s\n", token);
                }
                control_sum++;
                if (j >= IMAGE_HEIGHT) {
                    j = 0;
                    i++;
                    if (i >= IMAGE_WIDTH) { ;
                        printf("image width exceeded\n");
                        free(input_line);
                        return -1;
                    }
                }
                image[i][j++] = number;
            } else {
                printf("wrong val %s\n", token);
                free(input_line);
                return -1;
            }
            token = strtok(NULL, " ");
            if (token != NULL)
                number = atoi(token);
        }
    }
    if (control_sum != IMAGE_WIDTH * IMAGE_HEIGHT) {
        printf("Control sum and image resolution differs! Control sum: %d Img_res: %d\n", control_sum,
               IMAGE_WIDTH * IMAGE_HEIGHT);
        free(input_line);
        return -1;
    }
    free(input_line);
    return 0;
}

int write_result_file(int image[MAX_IMAGE_WIDTH][MAX_IMAGE_HEIGHT]) {
    fprintf(RESULT_FILE, "P2\n");
    fprintf(RESULT_FILE, "%d %d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    fprintf(RESULT_FILE, "%d\n", MAX_VAL);
    for(int i = 0; i < IMAGE_WIDTH; i++) {
        for(int j = 0; j < IMAGE_HEIGHT; j++) {
            fprintf(RESULT_FILE,"%d ",image[i][j]);
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    if (check_args_validity(argc, argv) == -1) return -1;
    int thread_number = atoi(argv[1]);
    char *mode = argv[2];
    int image[MAX_IMAGE_WIDTH][MAX_IMAGE_HEIGHT];
    if (read_image_file(image)) {
        puts("ERROR OCCURRED DURING FILE READING");
        close_files();
        return -1;
    }
    write_result_file(image);
    close_files();
}