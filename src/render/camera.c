#include "render/camera.h"
#include "log/log.h"
// TODO(sean) HANDMADE_MATH_USE_DEGREES once HMM no longer included in headers
#include <HandmadeMath.h>

#define MAX_PITCH 89.0f
#define MIN_PITCH -89.0f

void camera_init(Camera *camera, HMM_Vec3 init_pos) {
    camera->position = init_pos;
    camera->up = HMM_V3(0.0f, 1.0f, 0.0f);
    camera->forward = HMM_V3(0.0f, 0.0f, -1.0f);
    camera->yaw = -90.0f;
    camera->pitch = 0.0f;
}

void camera_rotate(Camera *camera, float delta_yaw, float delta_pitch) {
    camera->yaw += delta_yaw;
    camera->pitch += delta_pitch;

    if (camera->pitch > MAX_PITCH) {
        camera->pitch = MAX_PITCH;
    } else if (camera->pitch < MIN_PITCH) {
        camera->pitch = MIN_PITCH;
    }

    // TODO(sean) Remove HMM_AngleDeg once HANDMADE_MATH_USE_DEGREES in use
    camera->forward.X = HMM_CosF(HMM_AngleDeg(camera->yaw)) * HMM_CosF(HMM_AngleDeg(camera->pitch));
    camera->forward.Y = HMM_SinF(HMM_AngleDeg(camera->pitch));
    camera->forward.Z = HMM_SinF(HMM_AngleDeg(camera->yaw)) * HMM_CosF(HMM_AngleDeg(camera->pitch));
    camera->forward = HMM_NormV3(camera->forward);
}

void camera_translate(Camera *camera, HMM_Vec3 delta_pos) {
    camera->position = HMM_AddV3(camera->position, delta_pos);
}

HMM_Mat4 camera_calc_view(Camera *camera) {
    return HMM_LookAt_RH(camera->position, HMM_AddV3(camera->position, camera->forward),
                         camera->up);
}
