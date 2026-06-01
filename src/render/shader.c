#include "render/shader.h"
#include "log/log.h"
#include "platform/platform.h"
#include <glad/glad.h>
#include <stdlib.h>

// TODO(sean) Avoid hard-coding these?
#define VERT_SHADER_PATH "shaders/vertex.glsl"
#define FRAG_SHADER_PATH "shaders/fragment.glsl"

void render_create_shaders(ShaderProgram *shader_prog) {
    shader_prog->id = glCreateProgram();

    shader_prog->vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glAttachShader(shader_prog->id, shader_prog->vert_shader);

    shader_prog->frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glAttachShader(shader_prog->id, shader_prog->frag_shader);
}

static int render_reload_shader(unsigned int shader, const char *path) {
    char *shader_src = platform_read_entire_file(path);

    if (!shader_src) {
        log_error("Failed to read shader source code");
        return 1;
    }

    // Fixes a compiler warning
    const char *const_shader_src = shader_src;
    glShaderSource(shader, 1, &const_shader_src, NULL);

    free(shader_src);

    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char buf[512];
        glGetShaderInfoLog(shader, 512, NULL, buf);
        log_error("Compilation failed: %s", buf);
        return 1;
    }

    return 0;
}

int render_reload_shaders(ShaderProgram *shader_prog) {
    // TODO(sean) Error handling
    render_reload_shader(shader_prog->vert_shader, VERT_SHADER_PATH);
    render_reload_shader(shader_prog->frag_shader, FRAG_SHADER_PATH);

    glLinkProgram(shader_prog->id);

    GLint success;
    glGetProgramiv(shader_prog->id, GL_LINK_STATUS, &success);

    if (!success) {
        char buf[512];
        glGetProgramInfoLog(shader_prog->id, 512, NULL, buf);
        log_error("Linking failed: %s", buf);
        return 1;
    }

    return 0;
}
