#include <unistd.h>
#include <stdio.h>
#include <string.h>
int main() {
    
    int fd[2];
    pipe(fd);
    pid_t pid = fork();
    if (pid == 0) { // dziecko
        close(fd[1]);
        dup2(fd[0],STDIN_FILENO);
        execlp("grep", "grep","Ala", NULL);
    } else { // rodzic
        close(fd[0]);
        char text[] = "0 Ala ma kota'n Ala sadas\0\n";
        write(fd[1], text,strlen(text));
    }
    
    
    /*FILE* grep_input = popen("grep Ala", "w");
    fputs("Aala ma kots\n",grep_input);
    fputs("Asla nie ma as",grep_input);
    pclose(grep_input);*/

    /*int fd[2];
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