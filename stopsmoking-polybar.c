#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

volatile sig_atomic_t counter = 0, lastMax = __INT_MAX__, printflag = 1;

void printcounter() {
    if (printflag) {
        FILE *counterFile = fopen("/tmp/stopsmokingcounter", "w+");

        fprintf(counterFile, "%d", counter);
        fflush(counterFile);

        fclose(counterFile);
    }
}

// Signal handler for SIGUSR
void sigusr_handler(int signal) {
    if (signal == SIGUSR1) {
        if (counter > 0) {
            if (printflag)
                while (!__sync_val_compare_and_swap(&printflag, printflag, 0))
                    ;

            while (!__sync_val_compare_and_swap(&counter, counter, counter - 1))
                ;

            while (!__sync_val_compare_and_swap(&printflag, printflag, 1))
                ;

            printcounter();
        }
    }
}

void singleton() {
    FILE *fp;
    char path[1035];

    /* Open the command for reading. */
    fp = popen("pgrep stopsmoking-pol", "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        exit(EXIT_FAILURE);
    }

    /* Read the output a line at a time - output it. */
    int numProc = 0;
    while (fgets(path, sizeof(path), fp) != NULL) {
        if (++numProc >= 2) {
            printf("Another process is opened. EXIT");
            exit(EXIT_FAILURE);
        }
    }

    /* close */
    pclose(fp);
}

int main() {
    /* Singleton for process */
    singleton();

    struct sigaction sa;
    sa.sa_handler = sigusr_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    char *fifopath = "/tmp/fifostopsmoking";

    int fd = open(fifopath, O_RDONLY);
    if (fd < 0) {
        perror("Can't open pipe");
        exit(EXIT_FAILURE);
    }

    printcounter();

    for (int tries = 0; tries < 10; ++tries) {
        while (1) {
            char c;
            ssize_t sz = read(fd, &c, sizeof(char));
            if (sz == 0) {
                break;
            } else if (c != '\0') {
                tries = 0;

                if (c == 255) {
                    counter = 0;
                    printcounter();
                    lastMax = __INT_MAX__;
                } else if (c < lastMax) {
                    while (!__sync_val_compare_and_swap(&counter, counter, counter + 1))
                        ;
                    printcounter();
                    lastMax = c;
                }
            }
        }

        sleep(2);
    }

    close(fd);

    printf("NO SERVICE\n");

    exit(EXIT_SUCCESS);
}