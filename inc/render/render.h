#pragma once

// TODO(sean) Something better
#include "render/camera.h"
#include "render/shader.h"
#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


typedef struct {
    GLFWwindow *window;
    Camera camera;
    ShaderProgram shader_prog;
    bool use_wireframes;
} RenderContext;

extern RenderContext g_ctx;

int render_init(int width, int height);
RenderContext *render_get_context();
void render_toggle_wireframe();
void render_run();
void render_shutdown();
