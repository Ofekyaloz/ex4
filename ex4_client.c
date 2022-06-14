// Ofek Yaloz 206666729

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

#define SIZE 30
int flag = 1;

void sigHandler(int sig) {
    signal(SIGUSR1, sigHandler);
    alarm(0);
    char filename[SIZE] = "to_client_", *line = NULL;
    size_t len;
    pid_t pid = getpid();
    char *strPid = malloc(sizeof(pid_t) + 1);
    sprintf(strPid, "%d", pid);
    strcat(strcat(filename, strPid), ".txt");
    free(strPid);
    int stat, err = open(filename, O_RDONLY);
    if (err == -1) {
        printf("ERROR_FROM_EX4\n");
        return;
    }
    FILE *file = fopen(filename, "r");
    if (getline(&line, &len, file) != -1) {
        printf("%s\n", line);
    }
    if ((pid = fork()) < 0) {
        printf("ERROR_FROM_EX4\n");
        return;
    } else {
        if (pid == 0) {
            char *args[] = {"rm", filename, NULL};
            execvp("rm", args);
            printf("ERROR_FROM_EX4\n");
            exit(-1);
        } else {
            wait(&stat);
        }
    }
    flag =0;
}

void stopRunning(int sig) {
    signal(SIGALRM, stopRunning);
    printf("Client closed because no response was received from the server for 30 seconds\n");
    exit(-1);
}


int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("ERROR_FROM_EX4\n");
        return -1;
    }
    unsigned int charsWrite, len;
    int i, errors, counter = 0, server;
    while ((errors = open("to_srv.txt", O_WRONLY | O_TRUNC | O_CREAT | O_EXCL, S_IRUSR | S_IWGRP | S_IWUSR)) < 0) {
        ++counter;
        if (counter == 10) {
            printf("ERROR_FROM_EX4\n");
            return -1;
        }
        unsigned int tmp = rand() % 6;
        sleep(tmp);
    }
    if (errors < 0) {
        printf("ERROR_FROM_EX4\n");
        return -1;
    }


    FILE *fp = fopen("to_srv.txt", "w");
    pid_t pid = getpid();
    char *strPid = malloc(sizeof(pid_t) + 1);
    sprintf(strPid, "%d", pid);
    len = strlen(strPid);
    charsWrite = fwrite(strPid, 1, len, fp);
    free(strPid);
    if (charsWrite < len) {
        return -1;
    }

    charsWrite = fwrite("\n", 1, 1, fp);
    if (charsWrite < sizeof(char)) {
        return -1;
    }
    for (i = 1; i < 5; ++i) {
        char *tmp = argv[i];
        if (i == 1) {
            int j = 0;
            while (tmp[j] != '\0') {
                if (isdigit(tmp[j] < 0)) {
                    printf("ERROR_FROM_EX4\n");
                    exit(-1);
                }
                j++;
            }
            server = atoi(tmp);
            continue;
        }
        charsWrite = fwrite(tmp, 1, strlen(argv[i]), fp);
        if (charsWrite < sizeof(*argv[i])) {
            return -1;
        }
        charsWrite = fwrite("\n", 1, 1, fp);
        if (charsWrite < sizeof(char)) {
            return -1;
        }
    }
    fclose(fp);
    signal(SIGUSR1, sigHandler);
    signal(SIGALRM, stopRunning);
    kill(server, SIGUSR1);
    alarm(30);
    while (flag) {
        pause();
    }

    return 0;
}