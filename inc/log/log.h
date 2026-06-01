#pragma once

int log_init(const char *logfile_path);
void log_shutdown();

void log_error(const char *msg);
void log_warn(const char *msg);
void log_info(const char *msg);
void log_debug(const char *msg);
