#include "log/log.h"
#include "render/render.h"
#include <stdio.h>

#define INIT_WIDTH 800
#define INIT_HEIGHT 600

#define LOG_FILE_PATH "log.txt"

int main(int argc, char **argv) {
    if (log_init("log.txt") != 0) {
        printf("ERROR: Failed to initialize logging; aborting\n");
        return 1;
    }

    if (render_init(INIT_WIDTH, INIT_HEIGHT) != 0) {
        log_error("Failed to initialize rendering");
        log_shutdown();
        return 1;
    }

    log_info("Initialization successful");

    render_run();

    render_shutdown();
    log_shutdown();

    return 0;
}
