#include "render/shader.h"
#include "log/log.h"
#include "platform/platform.h"
#include <HandmadeMath.h>
#include <stdlib.h>
#include <glad/glad.h>

// TODO(sean) Avoid hard-coding these?
#define VERT_SHADER_PATH "shaders/vertex.glsl"
#define FRAG_SHADER_PATH "shaders/fragment.glsl"

void shader_create_shaders(ShaderProgram *shader_prog) {
    shader_prog->id = glCreateProgram();

    shader_prog->vert_shader.id = glCreateShader(GL_VERTEX_SHADER);
    glAttachShader(shader_prog->id, shader_prog->vert_shader.id);

    shader_prog->frag_shader.id = glCreateShader(GL_FRAGMENT_SHADER);
    glAttachShader(shader_prog->id, shader_prog->frag_shader.id);
}

static int shader_reload_shader(unsigned int shader_id, const char *path) {
    char *shader_src = platform_read_entire_file(path);

    if (!shader_src) {
        log_error("Failed to read shader source code");
        return 1;
    }

    // Fixes a compiler warning
    const char *const_shader_src = shader_src;
    glShaderSource(shader_id, 1, &const_shader_src, NULL);

    free(shader_src);

    glCompileShader(shader_id);

    GLint success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (!success) {
        // TODO(sean) Better formatting of info log
        char buf[512];
        glGetShaderInfoLog(shader_id, 512, NULL, buf);
        log_error("Shader compilation failed:\n%s", buf);
        return 1;
    }

    return 0;
}

int shader_reload_shaders(ShaderProgram *shader_prog) {
    if (shader_reload_shader(shader_prog->vert_shader.id, VERT_SHADER_PATH) != 0) {
        log_warn("Failed to reload vertex shader");
        return 1;
    }
    if (shader_reload_shader(shader_prog->frag_shader.id, FRAG_SHADER_PATH) != 0) {
        log_warn("Failed to reload fragment shader");
        return 1;
    }

    glLinkProgram(shader_prog->id);

    GLint success;
    glGetProgramiv(shader_prog->id, GL_LINK_STATUS, &success);

    if (!success) {
        // TODO(sean) Better formatting of info log
        char buf[512];
        glGetProgramInfoLog(shader_prog->id, 512, NULL, buf);
        log_error("Shader linking failed: %s", buf);
        return 1;
    }

    // TODO(sean) Error checking
    // Must be done *after* program has been linked
    shader_prog->vert_shader.model_mtx_loc = glGetUniformLocation(shader_prog->id, "model");
    shader_prog->vert_shader.view_mtx_loc = glGetUniformLocation(shader_prog->id, "view");
    shader_prog->vert_shader.project_mtx_loc = glGetUniformLocation(shader_prog->id, "project");

    return 0;
}

void shader_set_matrices(ShaderProgram *shader_prog, HMM_Mat4 model_mtx, HMM_Mat4 view_mtx,
                         HMM_Mat4 project_mtx) {
    glUniformMatrix4fv(shader_prog->vert_shader.model_mtx_loc, 1, GL_FALSE,
                       &model_mtx.Elements[0][0]);
    glUniformMatrix4fv(shader_prog->vert_shader.view_mtx_loc, 1, GL_FALSE,
                       &view_mtx.Elements[0][0]);
    glUniformMatrix4fv(shader_prog->vert_shader.project_mtx_loc, 1, GL_FALSE,
                       &project_mtx.Elements[0][0]);
}
