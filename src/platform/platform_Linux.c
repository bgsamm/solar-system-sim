#include "platform/platform.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PATH_SEP '/'

// Ref.: https://linuxvox.com/blog/how-do-i-find-the-location-of-the-executable-in-c/
const char *platform_get_exe_dir() {
    static char path_buf[PATH_MAX];

    ssize_t len = readlink("/proc/self/exe", path_buf, sizeof(path_buf) - 1);
    if (len == -1) {
        return NULL;
    } else if (len == PATH_MAX) {
        return NULL;
    }

    path_buf[len] = '\0';

    char *dir_sep = strrchr(path_buf, PATH_SEP);
    if (!dir_sep) {
        return NULL;
    }

    dir_sep[1] = '\0';

    return path_buf;
}

size_t platform_get_file_size(const char *path) {
    if (!path) {
        return -1;
    }

    struct stat st;

    if (stat(path, &st) != 0) {
        return -1;
    }

    return st.st_size;
}

char *platform_read_entire_file(const char *path) {
    if (!path) {
        return NULL;
    }

    size_t size = platform_get_file_size(path);

    if (size < 0) {
        return NULL;
    }

    FILE *file = fopen(path, "rb");

    if (!file) {
        return NULL;
    }

    char *buf = malloc(size + 1);

    if (!buf) {
        fclose(file);
        return NULL;
    }

    size_t sizeRead = fread(buf, 1, size, file);

    if (sizeRead < size) {
        if (ferror(file)) {
            free(buf);
            buf = NULL;
            goto end;
        }

        size = sizeRead;
    }

    buf[size] = '\0';

end:
    fclose(file);
    return buf;
}
