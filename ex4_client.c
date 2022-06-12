// Ofek Yaloz 206666729
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/random.h>
#include <string.h>
#include <sys/wait.h>

#define SIZE 100

void sigHandler(int sig) {
    signal(SIGUSR1, sigHandler);
    char filename[SIZE] = "to_client_", *text = NULL, *line;
    size_t len;
    pid_t pid = getpid();
    char *strPid = malloc(sizeof(pid_t) + 1);
    sprintf(strPid, "%d", pid);
    strcat(strcat(filename, strPid),".txt");
    free(strPid);
    int stat,err = open(filename, O_RDONLY);
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

}


int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("ERROR_FROM_EX4\n");
        return -1;
    }
    unsigned int charWrite, len;
    int i, errors, counter = 0;
    while ((errors = open("to_srv.txt", O_WRONLY | O_TRUNC | O_CREAT | O_EXCL, S_IRUSR | S_IWGRP | S_IWUSR)) < 0) {
        ++counter;
        if (counter == 10) {
            printf("ERROR_FROM_EX4\n");
            return -1;
        }
        unsigned int tmp;
        if (getrandom(&tmp, sizeof(unsigned int), GRND_NONBLOCK) == -1) {
            printf("ERROR_FROM_EX4\n");
            return -1;
        }
        sleep(tmp % 6);
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
    charWrite = fwrite(strPid, 1, len, fp);
    free(strPid);
    if (charWrite < len) {
        return -1;
    }

    charWrite = fwrite("\n", 1, 1, fp);
    if (charWrite < sizeof(char)) {
        return -1;
    }
    for (i = 2; i < 5; ++i) {
        char *tmp = argv[i];
        charWrite = fwrite(tmp, 1, strlen(argv[i]), fp);
        if (charWrite < sizeof(*argv[i])) {
            return -1;
        }
        charWrite = fwrite("\n", 1, 1, fp);
        if (charWrite < sizeof(char)) {
            return -1;
        }
    }
    fclose(fp);

    kill(*argv[1], SIGUSR1);
    signal(SIGUSR1, sigHandler);


//    printf("Client closed because no response was received from the server for 30 seconds\n");
    return 0;
}