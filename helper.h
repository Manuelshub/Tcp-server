#ifndef HELPER_H
#define HELPER_H

/* Header files */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

int handle_error(const char *message);
void sigchld_handler(int signum);
const char *get_log_timestamp();

#endif /* HELPER_H */