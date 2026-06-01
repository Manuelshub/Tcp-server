#include "helper.h"

int handle_error(const char *message) {
    perror(message);
    return EXIT_FAILURE;
}

void sigchld_handler(int signum) {
    (void) signum;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}