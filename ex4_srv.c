// Ofek Yaloz 206666729

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

#define SIZE 30
int flag = 1;

int removeFile(char *filename) {
    int stat;
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork failed\n");
        return -1;
    } else {
        if (pid == 0) {
            char *args[] = {"rm", filename, NULL};
            execvp("rm", args);
            exit(-1);
        } else {
            wait(&stat);
        }
    }
    return 0;
}


void *calc(void *arg) {
    size_t len;
    int input[4], j, i = 0, err = open("to_srv.txt", O_RDONLY);
    if (err == -1) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    FILE *file = fopen("to_srv.txt", "r");
    char filename[SIZE] = "to_client_", *text = NULL, *line = NULL;
    while (getline(&line, &len, file) != -1 && i < 4) {
        if (i == 0) {
            strcat(strcat(filename, line), ".txt");
        }
        j = 0;
        while (line[j] != '\n' || line[j] != '\0') {
            if (isdigit(line[j] < 0)) {
                removeFile(filename);
                printf("ERROR_FROM_EX4\n");
                exit(-1);
            }
            j++;
        }

        input[i] = atoi(line);
        ++i;
    }
    fclose(file);
    removeFile(filename);

    if (input[3] < 1 || input[3] > 4 || input[0] < 0) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    int resultfile = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWGRP | S_IWUSR);
    if (resultfile < 0) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    FILE *fp = fopen(filename, "w");
    text = malloc(sizeof(int));
    if (input[3] == 1) {
        sprintf(text, "%d", (input[2] + input[3]));
    } else if (input[3] == 2) {
        sprintf(text, "%d", (input[2] - input[3]));
    } else if (input[3] == 3) {
        sprintf(text, "%d", (input[2] * input[3]));
    } else {
        if (input[3] == 0) {
            char errmsg[SIZE] = "CANNOT_DIVIDE_BY_ZERO\n";
            fwrite(errmsg, 1, strlen(errmsg), fp);
            printf("ERROR_FROM_EX4\n");
            free(text);
            kill(input[0], SIGUSR1);
            exit(-1);
        }
        sprintf(text, "%d", (input[2] / input[3]));
    }
    fwrite(text, 1, strlen(text), fp);
    free(text);
    kill(input[0], SIGUSR1);
    return NULL;
}

void sigHandler(int sig) {
    printf("in sig\n");
    signal(SIGUSR1, sigHandler);
    alarm(60);
    calc(0);
    pthread_t pthread;
    int err = pthread_create(&pthread, NULL, calc, NULL);
    if (err != 0) {
        printf("ERROR_FROM_EX4\n");
    }
}

void stopRunning(int sig) {
    signal(SIGALRM, stopRunning);
    flag = 0;
}

int main() {
    int errors = open("to_srv.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWGRP | S_IWUSR);
    if (errors >= 0) {
        errors = removeFile("to_srv.txt");
        if (errors == -1) {
            printf("ERROR_FROM_EX4\n");
            return -1;
        }
    }
    signal(SIGUSR1, sigHandler);
    signal(SIGALRM, stopRunning);

    alarm(60);
    while (flag) {
        pause();
    }
    printf("The server was closed because no service request was received for the last 60 seconds\n");
    return 0;
}
