#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    struct passwd *pw = getpwuid(getuid());
    char *home = pw->pw_dir;
    char fifopath[64] = "";
    strcat(fifopath, home);
    strcat(fifopath, "/.config/stopsmoking/fifopipe");

    int fd = open(fifopath, O_RDONLY);
    if (fd < 0) {
        perror("Can't open pipe");
        exit(EXIT_FAILURE);
    }

    while (1) {
        while (1) {
            char c;
            ssize_t sz = read(fd, &c, sizeof(char));
            if (sz == 0) {
                break;
            } else if (c != '\0') {
                printf("Character: %d\n", c);
            }
        }

        sleep(5);
    }

    close(fd);

    exit(EXIT_SUCCESS);
}