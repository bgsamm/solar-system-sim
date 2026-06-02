#include "render/render.h"
#include "log/log.h"
#include "platform/platform.h"
#include "render/shader.h"
#include <stdbool.h>
#include <stddef.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef struct {
    GLFWwindow *window;
    bool use_wireframes;
    ShaderProgram shader_prog;
} RenderContext;

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

RenderContext ctx;

static void render_toggle_wireframe() {
    ctx.use_wireframes = !ctx.use_wireframes;
    glPolygonMode(GL_FRONT_AND_BACK, (ctx.use_wireframes ? GL_LINE : GL_FILL));

    if (ctx.use_wireframes) {
        log_debug("Wireframe mode: ON");
    } else {
        log_debug("Wireframe mode: OFF");
    }
}

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_W:
            if ((mods & GLFW_MOD_ALT)) {
                render_toggle_wireframe();
            } else if ((mods & GLFW_MOD_CONTROL)) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            break;

        case GLFW_KEY_R:
            if ((mods & GLFW_MOD_ALT)) {
                if (render_reload_shaders(&ctx.shader_prog) != 0) {
                    log_warn("Failed to reload shaders");
                } else {
                    log_info("Reloaded shaders");
                }
            }
            break;
        }
    }
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
    ctx.window = glfwCreateWindow(width, height, "Solar System", NULL, NULL);

    if (!ctx.window) {
        log_error("Failed to create GLFW window");
        return 1;
    }

    glfwMakeContextCurrent(ctx.window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        log_error("Failed to initialize GLAD");
        return 1;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(ctx.window, framebufferSizeCallback);

    glfwSetKeyCallback(ctx.window, keyCallback);

    return 0;
}

static int render_init_draw() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // TODO(sean) Remove this once we have actual stuff to render
    tri1 = render_create_vertex_array(tri1_verts, sizeof(tri1_verts));
    tri2 = render_create_vertex_array(tri2_verts, sizeof(tri2_verts));

    render_create_shaders(&ctx.shader_prog);
    if (render_reload_shaders(&ctx.shader_prog) != 0) {
        log_error("Failed to load shaders");
        return 1;
    }
    log_info("Loaded shaders");

    glUseProgram(ctx.shader_prog.id);

    // Render in fill mode by default
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    ctx.use_wireframes = false;

    return 0;
}

static void render_begin_frame() {
    glClear(GL_COLOR_BUFFER_BIT);
}

static void render_process_input() {
}

static void render_draw_frame() {
    glBindVertexArray(tri1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(tri2);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

static void render_end_frame() {
    glfwSwapBuffers(ctx.window);
    glfwPollEvents();
}

void render_run() {
    if (render_init_draw() != 0) {
        log_error("Failed to initialize draw");
        return;
    }

    while (!glfwWindowShouldClose(ctx.window)) {
        render_begin_frame();

        // TODO(sean) Move to input TU?
        render_process_input();

        render_draw_frame();

        render_end_frame();
    }

    // TODO(sean) glDeleteVertexArrays, glDeleteBuffers, glDeleteShader, glDeleteProgram
}

void render_shutdown() {
    glfwTerminate();
}
