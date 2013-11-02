#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>


static void sigusr1Handler(int sig) {
    printf("SIGUSR1 has been caught\n");
}


static void sigusr2Handler(int sig) {
    printf("SIGUSR2 has been caught\n");
}


static void sigintHandler(int sig) {
    printf("SIGINT has been caught, terminating the program\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char ** argv) {
    int pid = getpid();

    if (signal(SIGUSR1, sigusr1Handler) == SIG_ERR)
        exit(EXIT_FAILURE);

    if (signal(SIGUSR2, sigusr2Handler) == SIG_ERR)
        exit(EXIT_FAILURE);

    if (signal(SIGINT, sigintHandler) == SIG_ERR)
        exit(EXIT_FAILURE);

    kill(pid, SIGUSR1);
    kill(pid, SIGUSR2);
    kill(pid, SIGINT);

    return 0;
}
