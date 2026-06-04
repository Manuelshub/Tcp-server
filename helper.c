#include "helper.h"

int handle_error(const char *message) {
    perror(message);
    return EXIT_FAILURE;
}

/* Returns a formatted timestamp string for logging purposes */
const char *get_log_timestamp() {
    static char buffer[20];
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);

    if (strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm) == 0) {
        return "ERROR-TIME";
    }
    
    return buffer;
}