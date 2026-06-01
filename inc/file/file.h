#pragma once

#include <stddef.h>

size_t file_get_size(const char *path);
char *file_read_all(const char *path);
