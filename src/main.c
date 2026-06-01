#include "render/render.h"

#define INIT_WIDTH   800
#define INIT_HEIGHT  600

int main (int argc, char **argv) {
    // TODO(sean) Parse application directory from argv[0] (for shader search)

    int result = 0;

    if (render_init(800, 600) == 0) {
        render_run();
    } else {
        result = 1;
    }

    render_end();

    return result;
}
