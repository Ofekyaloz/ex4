// Ofek Yaloz 206666729

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 100

void *calc(void *arg) {
    size_t len;
    int input[4], i = 0, stat, err = open("to_srv.txt", O_RDONLY);
    if (err == -1) {
        printf("ERROR_FROM_EX4\n");
        return NULL;
    }
    FILE *file = fopen("to_srv.txt", "r");
    char filename[SIZE] = "to_client_", *text = NULL, *line;
    while (getline(&line, &len, file) != -1 && i < 4) {
        if (i == 0) {
            strcat(strcat(filename, line), ".txt");
        }
        input[i] = atoi(line);
        ++i;
    }
    fclose(file);
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork failed\n");
        return (void *) -1;
    } else {
        if (pid == 0) {
            char *args[] = {"rm", "to_srv.txt", NULL};
            execvp("rm", args);
            exit(-1);
        } else {
            wait(&stat);
        }
    }
    int resultfile = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWGRP | S_IWUSR);
    if (resultfile < 0) {

        return NULL;
    }
    FILE *fp = fopen(filename, "w");
    if (input[3] < 1 || input[3] > 4) {

        return NULL;
    }
    if (input[3] == 1) {
        text = malloc(sizeof(int));
        sprintf(text,"%d",(input[2] + input[3]));
    } else if (input[3] == 2) {
        text = malloc(sizeof(int));
        sprintf(text,"%d",(input[2] - input[3]));
    } else if (input[3] == 3) {
        text = malloc(sizeof(int));
        sprintf(text,"%d",(input[2] * input[3]));
    } else {
        text = malloc(sizeof(float));
        sprintf(text,"%f",((float) input[2] / (float) input[3]));
    }
    fwrite(text,1,strlen(text),fp);
    free(text);
    kill(input[0], SIGUSR1);
    return NULL;
}

void sigHandler(int sig) {
    signal(SIGUSR1, sigHandler);
    pthread_t pthread;
    int err = pthread_create(&pthread, NULL, calc, NULL);
    if (err != 0) {
        printf("ERROR_FROM_EX4\n");
    }
}

int main() {
    int flag = 1, stat, errors = open("to_srv.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWGRP | S_IWUSR);
    pid_t pid;
    if (errors >= 0) {
        if ((pid = fork()) < 0) {
            perror("fork failed\n");
            return -1;
        } else {
            if (pid == 0) {
                char *args[] = {"rm", "to_srv.txt", NULL};
                execvp("rm", args);
                exit(-1);
            } else {
                wait(&stat);
            }
        }
    }
    signal(SIGUSR1, sigHandler);
    while (flag) {

    }

    printf("The server was closed because no service request was received for the last 60 seconds\n");
    return 0;
}
