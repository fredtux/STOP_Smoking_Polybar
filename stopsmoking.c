#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

void logErrorMsg(char *msg) {
    char completeMsg[512] = "";

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    sprintf(completeMsg, "[%02d-%02d-%04d %02d:%02d:%02d] %s\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, msg);

    syslog(LOG_ALERT, "%s", completeMsg);

    if (write(STDERR_FILENO, completeMsg, strlen(completeMsg)) < 0) {
        exit(EXIT_FAILURE);
    }
}

void logOutputMsg(char *msg) {
    char completeMsg[512] = "";

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    sprintf(completeMsg, "[%02d-%02d-%04d %02d:%02d:%02d] %s\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, msg);

    syslog(LOG_NOTICE, "%s", completeMsg);

    if (write(STDOUT_FILENO, completeMsg, strlen(completeMsg)) < 0) {
        exit(EXIT_FAILURE);
    }
}

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
        logErrorMsg("Can't open config file. EXIT");
        exit(EXIT_FAILURE);
    }

    /* Read into variables */
    char tmp[256] = "";
    if (feof(confFile) || fscanf(confFile, "%[^=]=%u\n", tmp, cigarettes) != 2) {
        logErrorMsg("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }
    if (feof(confFile) || fscanf(confFile, "%[^=]=%u", tmp, starthour) != 2) {
        logErrorMsg("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }
    if (feof(confFile) || fscanf(confFile, "%[^=]=%u", tmp, finishhour) != 2) {
        logErrorMsg("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }
    if (feof(confFile) || fscanf(confFile, "%[^=]=%u", tmp, today) != 2) {
        logErrorMsg("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }
    if (feof(confFile) || fscanf(confFile, "%[^=]=%lu", tmp, dateadded) != 2) {
        logErrorMsg("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }
    if (feof(confFile) || fscanf(confFile, "%[^=]=%lu", tmp, dateupdated) != 2) {
        logErrorMsg("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }
	if (feof(confFile) || fscanf(confFile, "%[^=]=%lu", tmp, datelastquit) != 2) {
        logErrorMsg("Illegal config file. EXIT");
        fclose(confFile);
        exit(EXIT_FAILURE);
    }

	fclose(confFile);
}

void updateInConfig(unsigned cigarettes, unsigned starthour, unsigned finishhour, unsigned today, time_t dateadded, time_t dateupdated, time_t datelastquit) {
    /* Files */
    FILE *confFile;
    char home[64] = "/home/";
    strcat(home, getenv("USER"));
    char configFilePath[256] = "";

    /* Open config file */
    strcat(configFilePath, home);
    strcat(configFilePath, "/.config/stopsmoking/config");

    if ((confFile = fopen(configFilePath, "w")) == NULL) {
        logErrorMsg("Could not open config file. EXIT");
        exit(EXIT_FAILURE);
    }

    fprintf(confFile, "Cigarettes=%d\n", cigarettes);
    fprintf(confFile, "Starthour=%d\n", starthour);
    fprintf(confFile, "Finishhour=%d\n", finishhour);
    fprintf(confFile, "Today=%d\n", today);
    fprintf(confFile, "DateAdded=%lu\n", dateadded);

    time_t t = time(NULL);
    fprintf(confFile, "DateUpdated=%lu\n", t);
    fprintf(confFile, "DateLastQuit=%lu\n", datelastquit);

    fclose(confFile);
}

void updateToday(unsigned cigarettes, unsigned starthour, unsigned finishhour, unsigned *today, time_t dateadded, time_t dateupdated, time_t datelastquit) {
    *today = cigarettes;

    updateInConfig(cigarettes, starthour, finishhour, *today, dateadded, dateupdated, datelastquit);
}

void computeSleepTime(unsigned *sleepTime, unsigned *probabilityToSmoke, unsigned cigarettes, unsigned starthour, unsigned finishhour, unsigned today, time_t dateadded, time_t dateupdated, time_t datelastquit) {
    unsigned short remaining;

    /* Current time */
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    /* Date added */
    time_t t2 = dateadded;
    // struct tm tm2 = *localtime(&t2);

    /* Date updated */
    time_t t3 = dateupdated;
    struct tm tm3 = *localtime(&t3);


    if (t < t2) {  // Future => Exception
        logErrorMsg("Starts in the future!");
        exit(EXIT_FAILURE);
    } else if (tm3.tm_yday == tm.tm_yday && tm3.tm_year == tm.tm_year) {  // Today
        updateInConfig(cigarettes, starthour, finishhour, today, dateadded, dateupdated, datelastquit);
        remaining = finishhour - tm.tm_hour;
    } else {  // Older
        updateToday(cigarettes, starthour, finishhour, &today, dateadded, dateupdated, datelastquit);
        remaining = finishhour - tm.tm_hour;
    }

	*probabilityToSmoke = (unsigned)(60 * today / (remaining));
    *sleepTime = *probabilityToSmoke * 60;
}

void tryToSmoke(unsigned probabilityToSmoke, unsigned cigarettes, unsigned starthour, unsigned finishhour, unsigned *today, time_t dateadded, time_t dateupdated, time_t datelastquit) {
	srand(time(NULL));
	int smoke = (rand() % 100) < probabilityToSmoke;

	if(smoke){
		logOutputMsg("Smoke 1!!!");
		--(*today);
		updateInConfig(cigarettes, starthour, finishhour, *today, dateadded, dateupdated, datelastquit);
	}
}

void tryToQuit(unsigned probabilityToSmoke, time_t *datelastquit, unsigned *cigarettes, unsigned *starthour, unsigned *finishhour, unsigned *today, time_t *dateadded, time_t *dateupdated) {
	/* Current time */
    time_t t = time(NULL);
    // struct tm tm = *localtime(&t);

    /* Date last quit */
    time_t t2 = *datelastquit;
    // struct tm tm2 = *localtime(&t2);

	int diff = difftime(t, t2) * 60 * 60 * 24;

	if(diff >= 2){
		srand(time(NULL));
		int quit = (rand() % 100) < (double) probabilityToSmoke / 2;
		if(quit){
			logOutputMsg("Quitted 1 cigarette!!!");
			--(*cigarettes);
			*datelastquit = t;
			updateInConfig(*cigarettes, *starthour, *finishhour, *today, *dateadded, *dateupdated, *datelastquit);
		}
	}
}

int main(void) {
    /* Our process ID and Session ID */
    pid_t pid, sid;

    /* Smoking variables */
    unsigned cigarettes;
    unsigned starthour;
    unsigned finishhour;
    unsigned today;
    time_t dateadded;
    time_t dateupdated;
	time_t datelastquit;
	unsigned probabilityToSmoke;

    /* Sleep time */
    unsigned sleepTime = 2;

    /* Fork off the parent process */
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    /* If we got a good PID, then
       we can exit the parent process. */
    if (pid > 0) {
		logOutputMsg("Parent exited successfully");
        exit(EXIT_SUCCESS);
    }

    /* Change the file mode mask */
    umask(0);

    /* Open any logs here */
    // struct stat st = {0};
    // if (stat("/var/log/stopsmoking", &st) == -1) {
    //     mkdir("/var/log/stopsmoking", 0755);
    // }

    /* Create a new SID for the child process */
    sid = setsid();
    if (sid < 0) {
        /* Log the failure */
        logErrorMsg("Can't allocate SID. EXIT");
        exit(EXIT_FAILURE);
    }

    /* Change the current working directory */
    if ((chdir("/")) < 0) {
        /* Log the failure */
        logErrorMsg("Can't change cwd. EXIT");
        exit(EXIT_FAILURE);
    }

    /* Daemon-specific initialization goes here */
    readConfigData(&cigarettes, &starthour, &finishhour, &today, &dateadded, &dateupdated, &datelastquit);
    computeSleepTime(&sleepTime, &probabilityToSmoke, cigarettes, starthour, finishhour, today, dateadded, dateupdated, datelastquit);

    logOutputMsg("Service started successfully");

    /* The Big Loop */
    while (1) {
        /* Do some task here ... */
		logOutputMsg("Another try");
		tryToSmoke(probabilityToSmoke, cigarettes, starthour, finishhour, &today, dateadded, dateupdated, datelastquit);
		tryToQuit(probabilityToSmoke, &datelastquit, &cigarettes, &starthour, &finishhour, &today, &dateadded, &dateupdated);
		// exit(EXIT_SUCCESS);

        sleep(2); /* wait sleepTime seconds */
    }

    /* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    exit(EXIT_SUCCESS);
}