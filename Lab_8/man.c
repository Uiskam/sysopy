#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

int
main(void)
{
    FILE *stream;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    stream = fopen("./resources/mountain.ascii.pgm", "r");
    if (stream == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, stream)) != -1) {
        printf("Pobrano wiersz o długości %zu :\n", read);
        printf("%s", line);
    }

    free(line);
    fclose(stream);
    exit(EXIT_SUCCESS);
}
