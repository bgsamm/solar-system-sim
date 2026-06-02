#pragma once

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} LogLevel;

int log_init(const char *logfile_path);
void log_shutdown();

void log_write_message(LogLevel level, const char *msg, ...);

#define log_debug(msg, ...) log_write_message(LOG_DEBUG, msg, __VA_ARGS__)
#define log_info(msg, ...) log_write_message(LOG_INFO, msg, __VA_ARGS__)
#define log_warn(msg, ...) log_write_message(LOG_WARN, msg, __VA_ARGS__)
#define log_error(msg, ...) log_write_message(LOG_ERROR, msg, __VA_ARGS__)
