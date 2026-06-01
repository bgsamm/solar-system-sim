#pragma once

#include <stddef.h>

const char *platform_get_exe_location();
size_t platform_get_file_size(const char *path);
char *platform_read_entire_file(const char *path);
