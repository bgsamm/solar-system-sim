#include "log/log.h"
#include <stdio.h>

FILE *log_file = NULL;

int log_init(const char *log_file_path) {
    if (!log_file_path) {
        return 1;
    }

    log_file = fopen(log_file_path, "a");

    if (!log_file) {
        return 1;
    }

    return 0;
}

void log_shutdown() {
    if (log_file) {
        fclose(log_file);
    }
}

void log_error(const char *msg) {
    if (msg) {
        fprintf(log_file, "[ERROR] %s\n", msg);
    }
}

void log_warn(const char *msg) {
    if (msg) {
        fprintf(log_file, "[WARNING] %s\n", msg);
    }
}

void log_info(const char *msg) {
    if (msg) {
        fprintf(log_file, "[INFO] %s\n", msg);
    }
}

void log_debug(const char *msg) {
    if (msg) {
        fprintf(log_file, "[DEBUG] %s\n", msg);
    }
}
