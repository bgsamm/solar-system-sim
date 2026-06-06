#include "input/input.h"
#include "log/log.h"
#include "render/camera.h"
#include "render/render.h"
#include "render/shader.h"
#include <stdbool.h>
#include <GLFW/glfw3.h>

// TODO(sean) Name?
bool cursor_captured;

static void input_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
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
                if (shader_reload_shaders(&g_ctx.shader_prog) != 0) {
                    log_warn("Failed to reload shaders");
                } else {
                    log_info("Reloaded shaders");
                }
            }
            break;

        case GLFW_KEY_ESCAPE:
            glfwSetInputMode(g_ctx.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cursor_captured = false;
            break;
        }
    }
}

static void input_cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
    static bool is_first_input = true;
    static float last_x, last_y;

    if (!cursor_captured) {
        is_first_input = true;
        return;
    }

    if (is_first_input) {
        last_x = xpos;
        last_y = ypos;
        is_first_input = false;
    }

    float x_off = xpos - last_x;
    float y_off = -(ypos - last_y);
    last_x = xpos;
    last_y = ypos;

    const float sensitivity = 0.1f;
    x_off *= sensitivity;
    y_off *= sensitivity;

    camera_rotate(&g_ctx.camera, x_off, y_off);
}

static void input_mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (button) {
        case GLFW_MOUSE_BUTTON_1:
            if (!cursor_captured) {
                glfwSetInputMode(g_ctx.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                cursor_captured = true;
            }
            break;
        }
    }
}

void input_init() {
    cursor_captured = false;
    glfwSetKeyCallback(g_ctx.window, input_key_callback);
    glfwSetCursorPosCallback(g_ctx.window, input_cursor_pos_callback);
    glfwSetMouseButtonCallback(g_ctx.window, input_mouse_button_callback);
}

// TODO(sean) Move all this camera stuff out of here!!!
void input_update(float dt) {
    const float cam_speed = 5.f;
    float dx = cam_speed * dt;

    Camera *camera = &g_ctx.camera;

    if (glfwGetKey(g_ctx.window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_translate(camera, HMM_MulV3F(camera->up, dx));
    }
    if (glfwGetKey(g_ctx.window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_translate(camera,
                         HMM_MulV3F(HMM_NormV3(HMM_Cross(camera->forward, camera->up)), -dx));
    }
    if (glfwGetKey(g_ctx.window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_translate(camera, HMM_MulV3F(camera->up, -dx));
    }
    if (glfwGetKey(g_ctx.window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_translate(camera,
                         HMM_MulV3F(HMM_NormV3(HMM_Cross(camera->forward, camera->up)), dx));
    }
    if (glfwGetKey(g_ctx.window, GLFW_KEY_E) == GLFW_PRESS) {
        camera_translate(camera, HMM_MulV3F(camera->forward, dx));
    }
    if (glfwGetKey(g_ctx.window, GLFW_KEY_C) == GLFW_PRESS) {
        camera_translate(camera, HMM_MulV3F(camera->forward, -dx));
    }
}
