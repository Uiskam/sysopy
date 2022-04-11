#include <unistd.h>
#include <stdio.h>

int main() {

    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) { // dziecko
        close(fd[1]);
        int result = dup2(fd[0],STDIN_FILENO);
        printf("%d\n",result);
        execlp("grep", "grep","Ala", NULL);
    } else { // rodzic
        close(fd[0]);
        char text[] = "Ala ma kota";
        write(fd[1], text,sizeof(text)/sizeof(text[0]));
    }
    /*
     *
    FILE* grep_input = popen("grep Ala", "w");
    fputs("Ala ma kota\n Antek ma psa", grep_input);
    pclose(grep_input);

    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) { // dziecko
        close(fd[1]);
        char buf_read[50];
        read(fd[0], buf_read, sizeof (buf_read)/sizeof(buf_read[0]));
        printf("Reading from the pipe: %s\n",buf_read);
        return 0;
    } else { // rodzic
        close(fd[0]);
        write(fd[1],"Life is meaningless",1000);

    }

    int fd_children[2];
    pipe(fd_children);
    if (fork() == 0) {
        close(fd_children[0]);
        write(fd_children[1],"I feel so lonely!",1000);
    } else if (fork() == 0) {
        close(fd_children[1]);
        char buf_read[50];
        read(fd_children[0], buf_read, sizeof (buf_read)/sizeof(buf_read[0]));
        printf("Child to child communication test: %s\n",buf_read);
    }*/
}