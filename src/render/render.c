#include "render/render.h"
#include "file/file.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// TODO(sean) Avoid hard-coding these?
#define VERT_SHADER_PATH "shaders/vertex.glsl"
#define FRAG_SHADER_PATH "shaders/fragment.glsl"

typedef struct {
    GLFWwindow *window;
    bool use_wireframes;
    // TODO(sean) Pull out into shader struct?
    GLuint shader_prog;
    GLuint vert_shader;
    GLuint frag_shader;
} RenderContext;

const float tri1_verts[] = {
    -0.80f, -0.50f, 0.00f,
    -0.40f,  0.50f, 0.00f,
     0.00f, -0.50f, 0.00f
};

const float tri2_verts[] = {
     0.00f,  0.50f, 0.00f,
     0.40f, -0.50f, 0.00f,
     0.80f,  0.50f, 0.00f
};

GLuint tri1, tri2;

RenderContext ctx;

static int render_reload_shaders();

static void render_toggle_wireframe() {
    ctx.use_wireframes = !ctx.use_wireframes;
    glPolygonMode(GL_FRONT_AND_BACK, (ctx.use_wireframes ? GL_LINE : GL_FILL));
}

static void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
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
                render_reload_shaders();
            }
            break;
        }
    }
}

static GLuint render_create_vertex_array(const float *vertices, GLsizeiptr vertSize) {
    GLuint vertexArrayId;
    glGenVertexArrays(1, &vertexArrayId);
    glBindVertexArray(vertexArrayId);

    GLuint vertexBufferId;
    glGenBuffers(1, &vertexBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, vertSize, vertices, GL_STATIC_DRAW);

    // TODO(sean) Better understand index parameter
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    // TODO(sean) Reintroduce element array buffer?

    glBindVertexArray(0);

    return vertexArrayId;
}

// TODO(sean) Move shader code into its own TU?
static void render_create_shaders() {
    ctx.shader_prog = glCreateProgram();

    ctx.vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glAttachShader(ctx.shader_prog, ctx.vert_shader);

    ctx.frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glAttachShader(ctx.shader_prog, ctx.frag_shader);
}

static int render_reload_shader(GLuint shader, const char *path) {
    char *shader_src = file_read_all(path);

    if (!shader_src) {
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
        // TODO(sean) Write to log
        char buf[512];
        glGetShaderInfoLog(shader, 512, NULL, buf);
        printf("Compilation failed: %s\n", buf);
        return 1;
    }

    return 0;
}

static int render_reload_shaders() {
    // TODO(sean) Error handling
    render_reload_shader(ctx.vert_shader, VERT_SHADER_PATH);
    render_reload_shader(ctx.frag_shader, FRAG_SHADER_PATH);

    glLinkProgram(ctx.shader_prog);

    GLint success;
    glGetProgramiv(ctx.shader_prog, GL_LINK_STATUS, &success);

    if (!success) {
        // TODO(sean) Write to log
        char buf[512];
        glGetProgramInfoLog(ctx.shader_prog, 512, NULL, buf);
        printf("Linking failed: %s\n", buf);
        return 1;
    }

    return 0;
}

int render_init(int width, int height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // TODO(sean) Look into monitor & share params
    ctx.window = glfwCreateWindow(width, height, "Solar System", NULL, NULL);

    if (!ctx.window) {
        // TODO(sean) Log
        printf("Failed to create GLFW window\n");
        return 1;
    }

    glfwMakeContextCurrent(ctx.window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // TODO(sean) Log
        printf("Failed to initialize GLAD\n");
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

    // TODO(sean) Error handling
    render_create_shaders();
    render_reload_shaders();

    glUseProgram(ctx.shader_prog);

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
    // TODO(sean) Error handling
    render_init_draw();

    while (!glfwWindowShouldClose(ctx.window)) {
        render_begin_frame();

        // TODO(sean) Move to input TU?
        render_process_input();

        render_draw_frame();

        render_end_frame();
    }

    // TODO(sean) glDeleteVertexArrays, glDeleteBuffers, glDeleteShader, glDeleteProgram
}

void render_end() {
    glfwTerminate();
}
