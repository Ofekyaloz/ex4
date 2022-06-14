// Ofek Yaloz 206666729

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>

#define SIZE 30
int flag = 1;

void removeFile(char *filename) {
    int stat;
    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork failed\n");
        exit(-1);
    } else {
        if (pid == 0) {
            char *args[] = {"rm", filename, NULL};
            execvp("rm", args);
            exit(-1);
        } else {
            wait(&stat);
        }
    }
}


void calc() {
    size_t len;
    int input[4], charsWrite, j, i = 0, err = open("to_srv.txt", O_RDONLY);
    if (err == -1) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    FILE *file = fopen("to_srv.txt", "r");
    char filename[SIZE] = "to_client_", *text = NULL, *line = NULL;
    while (getline(&line, &len, file) != -1 && i < 4) {
        j = 0;
        while (line[j] != '\n') {
            if (isdigit(line[j] < 0)) {
                removeFile("to_srv.txt");
                printf("ERROR_FROM_EX4\n");
                exit(-1);
            }
            j++;
        }
        if (i == 0) {
            line[j] = '\0';
            strcat(strcat(filename, line), ".txt");
        }

        input[i] = atoi(line);
        ++i;
    }
    fclose(file);
    removeFile("to_srv.txt");

    if (input[2] < 1 || input[2] > 4 || input[0] < 0) {
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
    if (input[2] == 1) {
        sprintf(text, "%d", (input[1] + input[3]));
    } else if (input[2] == 2) {
        sprintf(text, "%d", (input[1] - input[3]));
    } else if (input[2] == 3) {
        sprintf(text, "%d", (input[1] * input[3]));
    } else {
        if (input[3] == 0) {
            char errmsg[SIZE] = "CANNOT_DIVIDE_BY_ZERO\n";
            fwrite(errmsg, 1, strlen(errmsg), fp);
            printf("ERROR_FROM_EX4\n");
            free(text);
            kill(input[0], SIGUSR1);
            exit(-1);
        }
        sprintf(text, "%d", (input[1] / input[3]));
    }
    len = strlen(text);
    charsWrite = write(resultfile, text, len);
    free(text);
    if (charsWrite < 0) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    charsWrite = fwrite("\n", 1, 1, fp);
    if (charsWrite < sizeof(char)) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    kill(input[0], SIGUSR1);
}

void sigHandler(int sig) {
    signal(SIGUSR1, sigHandler);
    alarm(60);
    pid_t pid;
    if ((pid = fork()) < 0) {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    } else {
        if (pid == 0) {
            calc();
        }
    }
}

void stopRunning(int sig) {
    signal(SIGALRM, stopRunning);
    flag = 0;
    int stat;
    while (wait(NULL) != -1);
}

int main() {
    int errors = open("to_srv.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWGRP | S_IWUSR);
    if (errors >= 0) {
      removeFile("to_srv.txt");
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
