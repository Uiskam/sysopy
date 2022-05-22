#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_IMAGE_WIDTH 1920
#define MAX_IMAGE_HEIGHT 1080
#define BILION 1000000000L

struct Thread_arg {
    int begin;
    int end;
};

FILE *IMAGE_FILE = NULL;
FILE *RESULT_FILE = NULL;
int IMAGE[MAX_IMAGE_WIDTH][MAX_IMAGE_HEIGHT];
int IMAGE_PROCESSED[MAX_IMAGE_WIDTH][MAX_IMAGE_HEIGHT];
int IMAGE_WIDTH;
int IMAGE_HEIGHT;
int MAX_VAL;
int MODE;

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

int read_image_file() {
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
                if (i >= IMAGE_WIDTH) {
                    i = 0;
                    j++;
                    if (j >= IMAGE_HEIGHT) { ;
                        printf("image width exceeded\n");
                        free(input_line);
                        return -1;
                    }
                }
                IMAGE[i++][j] = number;
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

int write_result_file() {
    fprintf(RESULT_FILE, "P2\n");
    fprintf(RESULT_FILE, "%d %d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    fprintf(RESULT_FILE, "%d\n", MAX_VAL);
    for (int j = 0; j < IMAGE_HEIGHT; j++) {
        for (int i = 0; i < IMAGE_WIDTH; i++){
            fprintf(RESULT_FILE, "%d ", IMAGE[i][j]);
        }
    }
    return 0;
}

int _ceil(int dividend, int divisor) {
    if(dividend % divisor == 0)
        return dividend / divisor;
    return dividend / divisor + 1;
}

int min(int a, int b) {
    return (a > b) ? b : a;
}

void* calculations(void* arg) {
    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);
    struct Thread_arg *thread_arg = (struct Thread_arg * ) arg;
    if (MODE == 1) {
        for (int y = 0; y < IMAGE_HEIGHT; y++) {
            for (int x = 0; x < IMAGE_WIDTH; x++) {
                if (thread_arg->begin <= IMAGE[x][y] && IMAGE[x][y] <= thread_arg->end && ++IMAGE_PROCESSED[x][y] == 1) {
                    IMAGE[x][y] = MAX_VAL - IMAGE[x][y];
                }
            }
        }
    } else if(MODE == 2) {
        for(int x  = thread_arg->begin; x <= thread_arg->end; x++) {
            for(int y = 0; y < IMAGE_HEIGHT; y++){
                IMAGE[x][y] = MAX_VAL - IMAGE[x][y];
            }
        }
    } else {
        printf("WRONG MODE %d\n",MODE);
        int err = -1;
        pthread_exit((void *)(&err));
    }
    clock_gettime(CLOCK_REALTIME, &end);
    double* elapsed_time = malloc(sizeof(double));
    *elapsed_time = (end.tv_sec - begin.tv_sec) + (double)(end.tv_nsec - begin.tv_nsec) / BILION;
    puts("thread exit");
    fflush(stdout);
    pthread_exit((void * )elapsed_time);
}

int main(int argc, char **argv) {
    for(int i =0 ; i < MAX_IMAGE_WIDTH; i++) {
        for(int j = 0; j < MAX_IMAGE_HEIGHT; j++) IMAGE_PROCESSED[i][j] = 0;
    }
    if (check_args_validity(argc, argv) == -1) return -1;
    if (strcmp(argv[2], "numbers") == 0) MODE = 1;
    else MODE = 2;

    if (read_image_file()) {
        puts("ERROR OCCURRED DURING FILE READING");
        close_files();
        return -1;
    }
    int thread_number = atoi(argv[1]);
    struct Thread_arg *thread_intervals = malloc(sizeof(struct Thread_arg) * thread_number);
    for (int k = 1; k <= thread_number; k++) {
        if (MODE == 1) {
            int interval_length = _ceil(MAX_VAL, thread_number);
            thread_intervals[k-1].begin = (k - 1) * interval_length;
            thread_intervals[k-1].end = min(k * interval_length - 1, MAX_VAL);
            printf("%d %d %d %d\n",k-1,interval_length, k ,interval_length );
        } else if(MODE == 2){
            int interval_length = _ceil(IMAGE_WIDTH, thread_number);
            thread_intervals[k-1].begin = (k - 1) * interval_length;
            thread_intervals[k-1].end = min(k * interval_length - 1, IMAGE_WIDTH - 1);
            //printf("%d %d %d %d\n",k-1,interval_length, k ,interval_length );
        } else {
            printf("WRONG MODE %d\n",MODE);
            close_files();
            free(thread_intervals);
            return -1;
        }
    }
    for(int i =0; i < thread_number; i++) printf("int no %d, begin: %d, end %d\n",i,thread_intervals[i].begin, thread_intervals[i].end);
    double total_real_time = 0;
    struct timespec begin, end;
    pthread_t* tid_tab = malloc(thread_number * sizeof (pthread_t));
    clock_gettime(CLOCK_REALTIME, &begin);
    for(int i = 0; i < thread_number; i++) {
        if(pthread_create(&tid_tab[i], NULL, calculations, (void *)&thread_intervals[i]) != 0) {
            perror("Thread start error");
            close_files();
            free(thread_intervals);
            free(tid_tab);
            return -1;
        }
    }

    for(int i = 0; i < thread_number; i++) {
        double* thread_time;
        if(pthread_join(tid_tab[i],(void *) &(thread_time)) != 0) {
            perror("thread join error");
            free(thread_time);
            close_files();
            free(thread_intervals);
            free(tid_tab);
            return -1;
        }
        printf("time of %d thread: %.9f\n", i, *thread_time);
        fflush(stdout);
        total_real_time += *thread_time;
        free(thread_time);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    total_real_time += (end.tv_sec - begin.tv_sec) + (double)(end.tv_nsec - begin.tv_nsec)/ BILION;
    printf("Total real time %.6lf\n", total_real_time);
    write_result_file();
    //free(tid_tab);
    //free(thread_intervals);
    close_files();
}