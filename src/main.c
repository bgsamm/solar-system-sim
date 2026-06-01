#include "platform/platform.h"
#include "render/render.h"
#include <stdio.h>

#define INIT_WIDTH   800
#define INIT_HEIGHT  600

int main (int argc, char **argv) {
    printf("Executable location: %s\n", platform_get_exe_location());

    // TODO(sean) Parse application directory from argv[0] (for shader search)

    int result = 0;

    if (render_init(INIT_WIDTH, INIT_HEIGHT) == 0) {
        render_run();
    } else {
        result = 1;
    }

    render_end();

    return result;
}
