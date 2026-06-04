#pragma once

// TODO(sean) I'd really like to avoid including HMM in headers
#include <HandmadeMath.h>

typedef struct {
    HMM_Vec3 position;
    HMM_Vec3 up;
    HMM_Vec3 forward;
    float yaw;
    float pitch;
} Camera;

void camera_init(Camera *camera, HMM_Vec3 init_pos);
void camera_rotate(Camera *camera, float delta_yaw, float delta_pitch);
void camera_translate(Camera *camera, HMM_Vec3 delta_pos);
HMM_Mat4 camera_calc_view(Camera *camera);
