
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mylib.h"

struct DataArray createDataArray(int size) {
    struct DataArray array;

    array.dataArray = (struct Block *) calloc(size, sizeof(struct Block));
    array.index = -1;
    return array;
};


int isNumber(char *s) {
    int i;
    for (i = 0; i < strlen(s); i++)
        if (!isdigit(s[i]))
            return 0;
    return 1;
}

void removeBlock(struct DataArray array, int index) {
    array.dataArray[index].line_count = -1;
    array.dataArray[index].word_count = -1;
    array.dataArray[index].char_count = -1;
};

int addToDataArray(struct DataArray array, struct Block newBlock) {
    array.dataArray[array.index + 1] = newBlock;
    return array.index + 1;
};

struct Block count_values(char *filename) {
    FILE *file;
    char ch = ' ', prev_ch = -1;
    int count_words = 0, count_lines = 0, count_characters = 0;
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Cannot open file");
        exit(1);
    }
    while (ch != EOF) {
        prev_ch = ch;
        ch = fgetc(file);
        //printf("%c %c %d %d\n",prev_ch,ch,(ch == EOF || ch ==' ' || ch == '\n' || ch == '\t'),(prev_ch != ' ' && prev_ch != '\t' && prev_ch != '\n'));
        if ((ch == EOF || ch == ' ' || ch == '\n' || ch == '\t') &&
            prev_ch != -1 && (prev_ch != ' ' && prev_ch != '\t' && prev_ch != '\n')) {
            count_words++;
        }
        if (ch == '\n')
            count_lines++;
        if (ch != -1)
            count_characters++;

    }
    fclose(file);
    struct Block stats;
    stats.char_count = count_characters;
    stats.line_count = count_lines;
    stats.word_count = count_words;
    return stats;
}