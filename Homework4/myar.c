#include <ar.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


void createFile(char *name, char *content, struct ar_hdr header) {
    int mode;
    int fd;
    sscanf(header.ar_mode, "%o", &mode);
    fd = creat(name, mode);
    if (fd == -1) {
        printf("error!");
        exit(EXIT_FAILURE);
    }
    write(fd, content, atoi(header.ar_size));
    close(fd);

}

void editHeaderName(char *name) {
    int k = sizeof(name);
    while (k > 0) {
        if (name[k] == '/') {
            name[k] = 0;
            break;
        }
        k--;
    }
}

void entryNotFound(int* found, int print, int numFiles, char** fileNames) {
    int i = 0;
    if (print == 0) {
        while (i < numFiles) {
            found[i] = 0;
            i++;
        }
    } else {
        i = 0;
        if (numFiles > 0) {
            while (i < numFiles) {
                if (found[i] == 0) {
                    printf("no entry %s in archive\n", fileNames[i]);
                }
                i++;
            }
        }
    }
}


void extract(int fd, int numFiles, char** fileNames) {
    struct ar_hdr header;
    int i = 0, file_length, j;
    int hdr_length = sizeof(struct ar_hdr);
    char headerName[15];
    int seek = 1;
    int found[numFiles];
    entryNotFound(found, 0, numFiles, fileNames);
    lseek(fd, SARMAG, SEEK_SET);
    while ((i < numFiles || numFiles == 0) &&
            (read(fd, (char*) &header, hdr_length)) == hdr_length) {
        j = 0;
        seek = 1;
        while (j < numFiles || numFiles == 0) {
            sscanf(header.ar_name, "%s", headerName);
            editHeaderName(headerName);
            if ((numFiles == 0) || (strcmp(fileNames[j], headerName) == 0)) {
                file_length = atoi(header.ar_size);
                char content[file_length];
                read(fd, content, file_length);
                createFile(headerName, content, header);
                lseek(fd, ((file_length % 2)), SEEK_CUR);
                seek = 0;
                found[i] = 1;
                i++;
                break;
            } else {
                j++;
            }
        }
        if (seek == 1) {
            file_length = atoi(header.ar_size);
            file_length += (file_length % 2);
            lseek(fd, file_length, SEEK_CUR);
        }
    }
    entryNotFound(found, 1, numFiles, fileNames);
}

void append(int fd, char* archive, int numFiles, char** fileNames) {
    int i = 0;
    lseek(fd, 0, SEEK_END);
    while (i < numFiles) {
        int file = open(fileNames[i], O_RDONLY);
        if (file < 0) {
            printf("myar: Cannot open %s\n", fileNames[i]);
            exit(EXIT_FAILURE);
        }
        struct stat file_stat;
        struct ar_hdr header;
        if (fstat(file, &file_stat) == 0) {
            char newName[15];
            sscanf(fileNames[i], "%s", newName);
            int len = strlen(newName);
            newName[len] = '/';
            newName[len+1] = '\0';
            sprintf(header.ar_name, "%-16.16s", newName);
            sprintf(header.ar_date, "%-12u", (uint32_t) file_stat.st_mtime);
            sprintf(header.ar_uid, "%-6u", (uint32_t) file_stat.st_uid);
            sprintf(header.ar_gid, "%-6u", (uint32_t) file_stat.st_gid);
            sprintf(header.ar_mode, "%-8o", (uint32_t) file_stat.st_mode);
            sprintf(header.ar_size, "%-10u", (uint32_t) file_stat.st_size);
            sprintf(header.ar_fmag, "%s", ARFMAG);
            write(fd, (char*) &header, sizeof(header));
        } else {
            printf("myar: Could not get status of %s\n", fileNames[i]);
            exit(EXIT_FAILURE);
        }
        char content[file_stat.st_size];
        read(file, content, sizeof(content));
        write(fd, content, sizeof(content));
        if ((file_stat.st_size % 2) == 1) {
           write(fd, "\n", 1);
        }
        close(file);
        i++;
    }
}

void delete(int fd, char* archive, int numFiles, char** fileNames) {
    struct ar_hdr header;
    int j, file_length, i = 0, seek = 1, copy_length = 0;
    int hdr_length = sizeof(struct ar_hdr);
    int found[numFiles];
    char headerName[15];

    if (numFiles == 0) {
        printf("myar: No filenames provided\n");
        exit(EXIT_FAILURE);
    }
    int temp = creat("temp", 0666);
    if (temp < 0) {
        printf("myar: Could not delete files\n");
        exit(EXIT_FAILURE);
    }
    entryNotFound(found, 0, numFiles, fileNames);
    write(temp, ARMAG, SARMAG);
    lseek(fd, SARMAG, SEEK_SET);
    while ((i < numFiles) && (read(fd, (char*) &header, hdr_length)) == hdr_length) {
        j = 0;
        seek = 1;
        copy_length += hdr_length;
        while (j < numFiles) {
            sscanf(header.ar_name, "%s", headerName);
            editHeaderName(headerName);
            if (strcmp(fileNames[j], headerName) == 0) {
                char content[copy_length];
                lseek(fd, -copy_length, SEEK_CUR);
                read(fd, content, (copy_length - hdr_length));
                write(temp, content, (copy_length - hdr_length));
                file_length = atoi(header.ar_size);
                file_length += (file_length % 2);
                lseek(fd, (file_length + hdr_length), SEEK_CUR);
                seek = 0;
                copy_length = 0;
                found[i] = 1;
                i++;
                break;
            } else {
                j++;
            }
        }
        if (seek == 1) {
            file_length = atoi(header.ar_size);
            file_length += (file_length % 2);
            lseek(fd, file_length, SEEK_CUR);
            copy_length += file_length;
        }
    }
    int pos = lseek(fd, 0, SEEK_CUR);
    int end = lseek(fd, 0, SEEK_END);
    copy_length += (end - pos);
    char content[copy_length];
    lseek(fd, -copy_length, SEEK_END);
    read(fd, content, copy_length);
    write(temp, content, copy_length);
    unlink(archive);
    link("temp", archive);
    unlink("temp");
    close(temp);
    entryNotFound(found, 1, numFiles, fileNames);
}


void octalToUnix(int mode) {
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
}

void appendDir(int fd, char* archive) {
    struct dirent* file;
    DIR* dir = opendir("./");
    if (dir != NULL) {
        while((file = readdir(dir))) {
            struct stat file_stat;
            char* fileName = file->d_name;
            if ((strcmp(fileName, ".") == 0) || (strcmp(fileName, "..") == 0)) {
               break;
            }
            if (stat(fileName, &file_stat) == 0) {
                if (!S_ISREG(file_stat.st_mode) || (fileName == archive)) {
                   break;
                }
            }
            append(fd, archive, 1, &fileName);
        }
        closedir(dir);
    } else {
        printf("myar: Could not open directory");
        exit(EXIT_FAILURE);
    }
}


void tableOfContents(int fd, char verbose) {
    char date[30];
    char headerName[15];
    int file_length, mode, uid, gid, size;
    int hdr_length = sizeof(struct ar_hdr);
    struct ar_hdr header;
    struct tm* timeinfo;
    lseek(fd, SARMAG, SEEK_SET);
    while ((read(fd, (char*) &header, hdr_length)) == hdr_length) {
        if (strcmp(header.ar_fmag, ARFMAG) == 0) {
            if (verbose == 'v') {
                sscanf(header.ar_mode, "%o", &mode);
                octalToUnix(mode);
                sscanf(header.ar_uid, "%d", &uid);
                sscanf(header.ar_gid, "%d", &gid);
                sscanf(header.ar_size, "%d", &size);
                time_t file_t = atoi(header.ar_date);
                timeinfo = localtime(&file_t);
                strftime(date, 30, "%b %d %R %Y", timeinfo);
                printf(" %-d/%-d %6d %s ", uid, gid, size, date);
            }
            sscanf(header.ar_name, "%s", headerName);
            editHeaderName(headerName);
            printf("%s\n", headerName);
        }
        file_length = atoi(header.ar_size);
        file_length += (file_length % 2);
        lseek(fd, file_length, SEEK_CUR);
    }
}

int main(int argc, char **argv) {
    char ch[SARMAG];
    int fd, num_read;
    //DIR* dir;
    char *archive, *option;

    if (argc < 3) {
        printf("myar: Not enough arguments\n");
        exit(EXIT_FAILURE);
    }
    option = argv[1];
    archive = argv[2];
    if (*option == '-') {
        option++;
    }
    if ((strcmp(option, "tv") != 0) && (strcmp(option, "vt") != 0)
            && strlen(option) > 1) {
        printf("myar: two different operation options specified\n");
        exit(EXIT_FAILURE);
    }

    fd = open(archive, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        printf("myar: %s: No such file or directory\n", archive);
        exit(EXIT_FAILURE);
    }

    umask(0);
    lseek(fd, 0, SEEK_SET);
    num_read = read(fd, ch, SARMAG);
    if (num_read < 0) {
        printf("myar: %s: Error while reading the file.\n", archive);
        exit(EXIT_FAILURE);
    } else if (num_read > 0) {
        if (strcmp(ch, ARMAG) != 0) {
            printf("myar: %s: File format not recognized\n", archive);
            exit(EXIT_FAILURE);
        }
    } else {
        if (*option == 'q') {
            printf("myar: %s: creating\n", archive);
            write(fd, ARMAG, SARMAG);
        } else {
            printf("myar: %s: Archive does not exist\n", archive);
            exit(EXIT_FAILURE);
        }
    }
    switch (*option) {
        case 'q':
            append(fd, archive, (argc - 3), (argv + 3));
            break;
        case 'x':
            extract(fd, (argc - 3), (argv + 3));
            break;
        case 't':
            if (strcmp(option, "tv") == 0) {
                tableOfContents(fd, 'v');
            } else {
                tableOfContents(fd, 't');
            }
            break;
        case 'v':
            tableOfContents(fd, 'v');
            break;
        case 'd':
            delete(fd, archive, (argc - 3), (argv + 3));
            break;
        case 'A':
            //appendDir(fd, archive);
            break;
        case 'w':
            break;
    }
    return 0;
}
