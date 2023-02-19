#include <argp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

const char *argp_program_version = "Stop Smoking with Polybar";
const char *argp_program_bug_address = "https://github.com/fredtux/STOP_Smoking_Polybar/issues";
static char doc[] = "Your program description.";
static char args_doc[] = "[FILENAME]...";
static struct argp_option options[] = {
    {"cigarettes", 'c', "c", 0, "Number of total cigarettes"},
    {"start_hour", 's', "s", 0, "The hour of the day when you want to start (24h format)"},
    {"finish_hour", 'f', "f", 0, "The hour of the day when you want to finish (24h format)"},
    {"today", 't', "t", 0, "How many cigarretes are remaining today"},
    {0}};

struct arguments {
    int CIGARETTES;
    int STARTHOUR;
    int FINISHHOUR;
    int TODAY;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'c':
            arguments->CIGARETTES = atoi(arg);
            break;
        case 's':
            arguments->STARTHOUR = atoi(arg);
            break;
        case 'f':
            arguments->FINISHHOUR = atoi(arg);
            break;
        case 't':
            arguments->TODAY = atoi(arg);
            break;
        case ARGP_KEY_ARG:
            return ARGP_ERR_UNKNOWN;
        case ARGP_KEY_END:
            // printf("arguments->wordlist : %s, arguments->url_to_query : %s\n", arguments->wordlist, arguments->url_to_query);
            if (arguments->CIGARETTES < 0 || arguments->STARTHOUR < 0 || arguments->FINISHHOUR < 0 || arguments->TODAY < 0) {
                argp_failure(state, 1, 0, "required -c, -s, -f and -t. See --help for more information");
                exit(ARGP_ERR_UNKNOWN);
            }
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

int main(int argc, char *argv[]) {
    FILE *confFile;
    struct passwd *pw = getpwuid(getuid());
    char *home = pw->pw_dir;
    char configDirPath[64] = "", configFilePath[64] = "";

    /* Parse arguments */
    struct arguments arguments;
    arguments.CIGARETTES = -1;
    arguments.STARTHOUR = -1;
    arguments.FINISHHOUR = -1;
    arguments.TODAY = -1;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    /* Check if start <= finish */
    if (arguments.STARTHOUR > arguments.FINISHHOUR) {
        printf("Start hour should be less or equal to finish hour");
        exit(ARGP_ERR_UNKNOWN);
    }

    /* Create directory for config file if it doesn't exist */
    strcat(configDirPath, home);
    strcat(configDirPath, "/.config/stopsmoking");
    struct stat st = {0};
    if (stat(configDirPath, &st) == -1) {
        if (mkdir(configDirPath, 0755) == -1) {
            perror("Could not create config directory at: ~/.config/stopsmoking");
            exit(EXIT_FAILURE);
        }
    }

    /* Open config file for write */
    strcat(configFilePath, configDirPath);
    strcat(configFilePath, "/config");
    if ((confFile = fopen(configFilePath, "w+")) == NULL) {
        perror("Could not open config file at: $HOME/.config/stopsmoking/config");
        exit(EXIT_FAILURE);
    }

    fprintf(confFile, "Cigarettes=%d\n", arguments.CIGARETTES);
    fprintf(confFile, "Starthour=%d\n", arguments.STARTHOUR);
    fprintf(confFile, "Finishhour=%d\n", arguments.FINISHHOUR);
    fprintf(confFile, "Today=%d\n", arguments.TODAY);

    time_t t = time(NULL);
    fprintf(confFile, "DateAdded=%lu\nDateUpdated=%lu\nDateLastQuit=%lu", t, t, t);
    
    fclose(confFile);

    exit(0);
}