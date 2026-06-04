#include "render/render.h"
#include "HandmadeMath.h"
#include "input/input.h"
#include "log/log.h"
#include "render/camera.h"
#include "render/shader.h"
#include <stdbool.h>
#include <stddef.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
    float position[3];
    float color[3];
} Vertex;

const Vertex tri1_verts[] = {
    {{-0.80f, -0.50f, 0.00f}, {1.0f, 0.0f, 0.0f}},
    {{-0.40f, 0.50f, 0.00f}, {0.0f, 1.0f, 0.0f}},
    {{0.00f, -0.50f, 0.00f}, {0.0f, 0.0f, 1.0f}},
};
const Vertex tri2_verts[] = {
    {{0.00f, 0.50f, 0.00f}, {1.0f, 0.0f, 0.0f}},
    {{0.40f, -0.50f, 0.00f}, {0.0f, 1.0f, 0.0f}},
    {{0.80f, 0.50f, 0.00f}, {0.0f, 0.0f, 1.0f}},
};

GLuint tri1, tri2;

RenderContext g_ctx;

// TODO(sean) Better place for these?
HMM_Mat4 model_mtx, view_mtx, project_mtx;

static void render_framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

static GLuint render_create_vertex_array(const Vertex *vertices, GLsizeiptr size) {
    GLuint vertex_array_id;
    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    GLuint vertex_buffer_id;
    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void *)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    // TODO(sean) Reintroduce element array buffer?

    glBindVertexArray(0);

    return vertex_array_id;
}

int render_init(int width, int height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // TODO(sean) Look into monitor & share params
    g_ctx.window = glfwCreateWindow(width, height, "Solar System", NULL, NULL);

    if (!g_ctx.window) {
        log_error("Failed to create GLFW window");
        return 1;
    }

    glfwMakeContextCurrent(g_ctx.window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        log_error("Failed to initialize GLAD");
        return 1;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(g_ctx.window, render_framebuffer_size_callback);

    return 0;
}

void render_toggle_wireframe() {
    g_ctx.use_wireframes = !g_ctx.use_wireframes;
    glPolygonMode(GL_FRONT_AND_BACK, (g_ctx.use_wireframes ? GL_LINE : GL_FILL));

    if (g_ctx.use_wireframes) {
        log_debug("Wireframe mode: ON");
    } else {
        log_debug("Wireframe mode: OFF");
    }
}

static int render_init_draw() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // TODO(sean) Remove this once we have actual stuff to render
    tri1 = render_create_vertex_array(tri1_verts, sizeof(tri1_verts));
    tri2 = render_create_vertex_array(tri2_verts, sizeof(tri2_verts));

    shader_create_shaders(&g_ctx.shader_prog);
    if (shader_reload_shaders(&g_ctx.shader_prog) != 0) {
        log_error("Failed to load shaders");
        return 1;
    }
    log_info("Loaded shaders");

    glUseProgram(g_ctx.shader_prog.id);

    model_mtx = HMM_M4D(1.0f);
    view_mtx = HMM_M4D(1.0f);
    // TODO(sean) Move this somewhere sensible (& #define values or smth)
    project_mtx = HMM_Perspective_RH_NO(HMM_AngleDeg(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    camera_init(&g_ctx.camera, HMM_V3(0.0f, 0.0f, 3.0f));

    // Render in fill mode by default
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    g_ctx.use_wireframes = false;

    return 0;
}

static void render_begin_frame() {
    glClear(GL_COLOR_BUFFER_BIT);
}

static void render_draw_frame() {
    view_mtx = camera_calc_view(&g_ctx.camera);
    shader_set_matrices(&g_ctx.shader_prog, model_mtx, view_mtx, project_mtx);

    glBindVertexArray(tri1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(tri2);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

static void render_end_frame() {
    glfwSwapBuffers(g_ctx.window);
    glfwPollEvents();
}

// TODO(sean) Move out of render.c
void render_run() {
    if (render_init_draw() != 0) {
        log_error("Failed to initialize draw");
        return;
    }

    input_init();

    float t0 = glfwGetTime();
    float dt = 0.0;

    while (!glfwWindowShouldClose(g_ctx.window)) {
        render_begin_frame();

        float t1 = glfwGetTime();
        dt = t1 - t0;
        t0 = t1;

        input_update(dt);

        render_draw_frame();

        render_end_frame();
    }

    // TODO(sean) glDeleteVertexArrays, glDeleteBuffers, glDeleteShader, glDeleteProgram
}

void render_shutdown() {
    glfwTerminate();
}
