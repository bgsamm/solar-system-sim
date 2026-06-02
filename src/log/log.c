#include "log/log.h"
#include <stdarg.h>
#include <stdio.h>

FILE *log_file = NULL;

int log_init(const char *log_file_path) {
    if (!log_file_path) {
        return 1;
    }

    log_file = fopen(log_file_path, "w");

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

void log_write_message(LogLevel level, const char* msg, ...) {
    if (!msg) {
        return;
    }

    va_list args1, args2;
    va_start(args1, msg);
    va_copy(args2, args1);

    const char *tag;
    switch (level) {
    case LOG_DEBUG:
        tag = "DEBUG";
        break;
    case LOG_INFO:
        tag = "INFO";
        break;
    case LOG_WARN:
        tag = "WARNING";
        break;
    case LOG_ERROR:
        tag = "ERROR";
        break;
    }

    printf("[%s] ", tag);
    vprintf(msg, args1);
    printf("\n");
    va_end(args1);

    if (log_file) {
        fprintf(log_file, "[%s] ", tag);
        vfprintf(log_file, msg, args2);
        fprintf(log_file, "\n");
    }
    va_end(args2);
}
