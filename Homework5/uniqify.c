#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#define MAXLEN 64

void sort(char **words, int n);
static void sigintHandler(int sig);
void merge(int *mqpid, int num_processes, int num_words);
unsigned long parse(FILE *input, int *mqpid, int num_processes);

int *mqpidWords;
int *mqpidSorted;
int num_processes;

struct mesg {
    long mtype;
    char msg[MAXLEN];
};

static void sigintHandler(int sig) {
    int i;
    for (i = 0; i < num_processes; i++) {
        msgctl(mqpidWords[i], IPC_RMID, NULL);
        msgctl(mqpidSorted[i], IPC_RMID, NULL);
    }
    exit(EXIT_FAILURE);
}


void merge(int *mqpid, int num_proc, int num_words) {
    struct mesg ptr[num_processes];
    char word[MAXLEN];
    int i = 0, j = 0, read = 0, wc = 1;
    for (i = 0; i < num_proc; i++) {
        if (msgrcv(mqpid[i], &ptr[i], MAXLEN, (i+1), 0) == -1) {
            exit(EXIT_FAILURE);
        }
    }
    i = 0;
    while(read < num_words) {
        wc = 0;
        j = 0;
        strcpy(word, ptr[i].msg);
        //Find alphabetically first word
        while (wc == 0 && j < num_processes) {
            if (i == j) {
                j++;
            }
            if (strcmp(word, "~~~") == 0) {
                wc = -1;
            } else {
                if (j < num_proc && strcmp(ptr[j].msg, "~~~") != 0) {
                    if (strcmp(word, ptr[j].msg) > 0) {
                        wc = -1;
                        i = j-1;
                    } else {
                        j++;
                    }
                } else {
                    j++;
                }
            }
        }
        //Find duplicates and print
        if (wc >= 0) {
            j = 0;
            while (j < num_proc) {
                if (strcmp(word, ptr[j].msg) == 0) {
                    wc++;
                    if (msgrcv(mqpid[j], &ptr[j], MAXLEN, (j+1), 0) == -1) {
                        exit(EXIT_FAILURE);
                    } else {
                    }
                    read++;
                } else {
                    j++;
                }
            }
          printf("%7d %s\n", wc, word);
        } else {
            i = (i + 1) % num_proc;
        }
    }
}

unsigned long parse(FILE *input, int *mqpid, int num_proc) {
    char str[MAXLEN];
	unsigned long count = 0;
	int procNum = 0, i = 0;
    char *token;
    struct mesg ptr;
    // Threw everything into the delimiter
    const char delimiter[] = " \n!\"#$%,.?][-+_\\/<>=@^:;&')(*1234567890";
	while (scanf("%s", str) == 1) {
		if (strncmp(str, "\n", 1) != 0) {
            token = strtok(str, delimiter);
            while (token != NULL) {
                for (i = 0; token[i]; i++) {
                    token[i] = tolower(token[i]);
                }
                ptr.mtype = (procNum + 1);
                strcpy(ptr.msg, token);
                msgsnd(mqpid[procNum], &ptr, MAXLEN, 0);
			    procNum = (procNum+1) % num_processes;
                token = strtok(NULL, delimiter);
			    count++;
            }
        }
	}
    //Send last message which will fail strcmp
    strcpy(ptr.msg, "~~~");
    for (i = 0; i < num_proc; i++) {
        ptr.mtype = (i + 1);
        msgsnd(mqpid[i], &ptr, MAXLEN, 0);
    }
	return count;
}

void sort(char **words, int n) {
    char *temp;
    int i = 0, j = 0;
    temp = (char*)malloc(MAXLEN*sizeof(char));
    for (i = 0; i < n; i++) {
        for (j = 0; j < n-1; j++) {
            if (strcmp(words[j], words[j+1]) > 0) {
                strcpy(temp, words[j]);
                strcpy(words[j], words[j+1]);
                strcpy(words[j+1], temp);
            }
        }
    }
}


int main(int argc, char **argv) {
    char *option;
    int parentPID = getpid();
    int i = 0;
    struct mesg toSort;
    int j = 0;
    char **words;
    int max_words = 8192;
    int k = 0;
    long unsigned int wordCount;

    if (argc == 1) {
        num_processes = 1;
    } else if (argc == 3){
        option = argv[1];
        if (*option == '-') {
            option++;
        }
        if (*option != 'n') {
            perror("Not a valid argument\n");
            exit(EXIT_FAILURE);
        }
        num_processes = atoi(argv[2]);
        if (num_processes > 50) {
            perror("Uniqify does not support over 50 processes");
            exit(EXIT_FAILURE);
        }
        if (num_processes < 0) {
            perror("Please enter a positive integer");
            exit(EXIT_FAILURE);
        }
    } else {
        perror("Please include the number of processes with -n or no arguments for 1 process\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGINT, sigintHandler) == SIG_ERR) {
        exit(EXIT_FAILURE);
    }

    mqpidWords = malloc(num_processes*sizeof(int));
    mqpidSorted = malloc(num_processes*sizeof(int));
    for (i = 0; i < num_processes; i++) {
        mqpidWords[i] = msgget(IPC_PRIVATE, 0666|IPC_CREAT);
        mqpidSorted[i] = msgget(IPC_PRIVATE, 0666|IPC_CREAT);
    }
    i = 0;
    words = malloc(max_words*sizeof(char*));
    while (getpid() == parentPID && i < num_processes) {
        if (mqpidWords[i] == -1 || mqpidSorted[i] == -1) {
            perror("Failed to create message queues");
            exit(EXIT_FAILURE);
        }
        switch (fork()) {
            case -1:
                perror("Failed to fork");
                exit(EXIT_FAILURE);
            case 0:
                j = 0;
                while (strcmp(toSort.msg, "~~~") != 0) {
                    if (msgrcv(mqpidWords[i], &toSort, MAXLEN, (i+1), 0) != -1) {
                        if (j >= max_words) {
                            max_words = max_words*2;
                            words = realloc(words, max_words);
                        }

                        words[j] = malloc((MAXLEN+1)*sizeof(char));
                        strcpy(words[j], toSort.msg);
                        j++;
                    }
                }
                sort(words, j);
                toSort.mtype = (i + 1);
                k = 0;
                while (k < j) {
                    strcpy(toSort.msg, words[k]);
                    msgsnd(mqpidSorted[i], &toSort, MAXLEN, 0);
                    k++;
                }
                exit(EXIT_SUCCESS);
            default:
                break;
        }
        i++;
    }
    wordCount = parse(stdin, mqpidWords, num_processes);
    merge(mqpidSorted, num_processes, wordCount);
    for (i = 0; i < num_processes; i++) {
        wait(NULL);
    }
    for (i = 0; i < num_processes; i++) {
        msgctl(mqpidWords[i], IPC_RMID, NULL);
        msgctl(mqpidSorted[i], IPC_RMID, NULL);
    }
    return 0;
}
