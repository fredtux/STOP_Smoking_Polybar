#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

volatile sig_atomic_t counter = 0, lastMax = __INT_MAX__, printflag = 1;

void readConfigData(unsigned *cigarettes, unsigned *starthour, unsigned *finishhour, unsigned *today, time_t *dateadded, time_t *dateupdated, time_t *datelastquit) {
    /* Files */
    FILE *confFile;
    char home[64] = "/home/";
    strcat(home, getenv("USER"));
    char configFilePath[256] = "";

    /* Open config file */
    strcat(configFilePath, home);
    strcat(configFilePath, "/.config/stopsmoking/config");

    if ((confFile = fopen(configFilePath, "r")) == NULL) {
        printf("Can't open config file. EXIT");
        exit(EXIT_FAILURE);
    }

    /* Read into variables */
    char tmp[256] = "";
    if (feof(confFile) || fscanf(confFile, "%[^=]=%u\n", tmp, cigarettes) != 2) {
        printf("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }
    if (feof(confFile) || fscanf(confFile, "%[^=]=%u", tmp, starthour) != 2) {
        printf("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }
    if (feof(confFile) || fscanf(confFile, "%[^=]=%u", tmp, finishhour) != 2) {
        printf("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }
    if (feof(confFile) || fscanf(confFile, "%[^=]=%u", tmp, today) != 2) {
        printf("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }
    if (feof(confFile) || fscanf(confFile, "%[^=]=%lu", tmp, dateadded) != 2) {
        printf("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }
    if (feof(confFile) || fscanf(confFile, "%[^=]=%lu", tmp, dateupdated) != 2) {
        printf("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }
    if (feof(confFile) || fscanf(confFile, "%[^=]=%lu", tmp, datelastquit) != 2) {
        printf("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }

    fclose(confFile);
}

void initCounter(){
    /* Smoking variables */
    unsigned cigarettes;
    unsigned starthour;
    unsigned finishhour;
    unsigned today;
    time_t dateadded;
    time_t dateupdated;
    time_t datelastquit;

    readConfigData(&cigarettes, &starthour, &finishhour, &today, &dateadded, &dateupdated, &datelastquit);

    /* Current time */
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    /* Date updated */
    time_t t2 = dateupdated;
    struct tm tm2 = *localtime(&t2);

    if(tm2.tm_yday == tm.tm_yday || tm2.tm_year == tm.tm_year){
        int nowCounter = -1;

        /* Files */
        char home[64] = "/home/";
        strcat(home, getenv("USER"));
        char counterFile[256] = "";

        /* Open config file */
        strcat(counterFile, home);
        strcat(counterFile, "/.config/stopsmoking/stopsmokingcounter");

        FILE *cf = fopen(counterFile, "r");
        if(cf == (void *)0)
            return;

        fscanf(cf, "%d", &nowCounter);
        if(nowCounter > 0)
            counter = nowCounter;
        fclose(cf);
    }
}

void printcounter() {
    if (printflag) {
        /* Files */
        char home[64] = "/home/";
        strcat(home, getenv("USER"));
        char counterFile[256] = "";

        /* Open config file */
        strcat(counterFile, home);
        strcat(counterFile, "/.config/stopsmoking/stopsmokingcounter");

        FILE *cf = fopen(counterFile, "w+");

        fprintf(cf, "%d", counter);
        fflush(cf);

        fclose(cf);
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

    initCounter();
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
                    if(counter == 0)
                        while (!__sync_val_compare_and_swap(&counter, counter, 1))
                        ;
                    else
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