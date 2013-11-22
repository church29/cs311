#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>

void closeAll(int pfd[4][2], int i);


void closeAll(int pfd[4][2], int i) {
    int j = 0;
    while (j <= i) {
        if (close(pfd[j][0]) == -1) {
            exit(EXIT_FAILURE);
        }
        if (close(pfd[j][1]) == -1) {
            exit(EXIT_FAILURE);
        }
        j++;
    }
}


int main(int argc, char **argv) {
    char *option, *file_name;
    int i = 0, num_pipes = 4, parent=getpid();
    int pfd[num_pipes][2];

    if (argc != 3) {
        perror("Please include a file with the -f command\n");
        exit(EXIT_FAILURE);
    } else {
        option = argv[1];
        file_name = argv[2];
    }

    if (*option == '-') {
        option++;
    }

    if (*option != 'f') {
        perror("Not a valid argument\n");
        exit(EXIT_FAILURE);
    }
    while (getpid() == parent && i <= num_pipes) {
        if (pipe(pfd[i]) == -1) {
            exit(1);
        }
        switch (fork()) {
            case -1:
                exit(EXIT_FAILURE);
            case 0:
                if (i == 0) {
                    // initial, redirect stdout to pipe out
                    if (pfd[i][1] != STDOUT_FILENO) {
                        if (dup2(pfd[i][1], STDOUT_FILENO) == -1) {
                            exit(EXIT_FAILURE);
                        }
                    }
                } else if (i == num_pipes) {
                    // end, redirect stdin to previous pipe in
                    if (pfd[i-1][0] != STDIN_FILENO) {
                        if (dup2(pfd[i-1][0], STDIN_FILENO) == -1) {
                            exit(EXIT_FAILURE);
                        }
                    }
                } else {
                    //redirect stdin to previous pipe in, and stdout to pipe out
                    if (pfd[i-1][0] != STDIN_FILENO) {
                        if (dup2(pfd[i-1][0], STDIN_FILENO) == -1) {
                            exit(EXIT_FAILURE);
                        }
                    }
                    if (pfd[i][1] != STDOUT_FILENO) {
                        if (dup2(pfd[i][1], STDOUT_FILENO) == -1) {
                            exit(EXIT_FAILURE);
                        }
                    }
                }
                closeAll(pfd, i);
                switch(i) {
                    case 0:
                        execlp("rev", "rev", NULL);
                    case 1:
                        execlp("sort", "sort", NULL);
                    case 2:
                        execlp("uniq", "uniq", "-c", NULL);
                    case 3:
                        execlp("tee", "tee", file_name, NULL);
                    case 4:
                        execlp("wc", "wc", NULL);
                }
                exit(EXIT_FAILURE);
            default:
                break;

        }
        i++;
    }
    closeAll(pfd, num_pipes);
    while (i > 0) {
        if (wait(NULL) == -1) {
            exit(1);
        }
        i--;
    }
    return 0;
    exit(EXIT_SUCCESS);

}
