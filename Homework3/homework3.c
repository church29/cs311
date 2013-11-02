#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

int main(int argc, char **argv)
{
    int vflag, Oflag, eflag, len, elen;
    int offset1 = 0;
    int offset2 = 0;
    int fd = -1;
    char c;
    struct stat s;
    opterr = 0;

    while((c = getopt (argc, argv, "vf:l:o:O:e:")) != -1) {
        switch (c) {
            case 'f':
                if (stat(optarg, &s) == 0) {
                    fd = open(optarg, O_RDONLY);
                } else {
                    printf("file '%s' not found\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                vflag = 1;
                break;
            case 'l':
                if ((sscanf(optarg, "%d", &len) != 0) && (len > 0)) {
                } else {
                    printf("The argument, '%s', you gave for l (length) is "
                           "not a postive integer\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'o':
                if ((sscanf(optarg, "%d", &offset1) != 0) && (offset1 > 0)) {
                } else {
                    printf("The argument, '%s', you gave for o (offset1) is "
                           "not a positive integer\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'O':
                if (sscanf(optarg, "%d", &offset2) != 0) {
                    Oflag = 1;
                } else {
                    printf("The argument, '%s', you gave for O (offset2) is "
                           "not an integer\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'e':
                if (sscanf(optarg, "%d", &elen) != 0) {
                    eflag = 1;
                } else {
                    printf("The argument, '%s', you gave for e (elen) is "
                           "not an integer\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
        }
    }
    if (fd < 0) {
        printf("No file given. Please input a file with the -f flag.\n");
        exit(EXIT_FAILURE);
    }
    if (vflag == 1) {
        while (--argc >= 0) {
            printf("%s ", *argv++);
        }
        printf("\n\n");
    }
    char read_char[len];
    lseek(fd, offset1, SEEK_SET);
    read(fd, read_char, len);
    printf("<offset 1> -----------------------------------------------\n");
    write(STDOUT_FILENO, read_char, len);
    if (read_char[len-1] != 10) {
        printf("\n");
    }
    if (Oflag == 1) {
        lseek(fd, offset2, SEEK_CUR);
        read(fd, read_char, len);
        printf("<offset 2> -----------------------------------------------\n");
        write(STDOUT_FILENO, read_char, len);
        if (read_char[len-1] != 10) {
            printf("\n");
        }
    }
    if (eflag == 1) {
        char end_read_char[abs(elen)];
        lseek(fd, -elen, SEEK_END);
        read(fd, end_read_char, abs(elen));
        printf("<end bytes> ----------------------------------------------\n");
        write(STDOUT_FILENO, end_read_char, abs(elen));
        if (end_read_char[abs(elen)-1] != 10) {
            printf("\n");
        }
    }
    printf("----------------------------------------------------------\n");
    return 0;
}
