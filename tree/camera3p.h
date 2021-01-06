#pragma once

#include <cglm/struct.h>

typedef struct {
    vec3s target;
    vec3s up;
    float distance;
    float pitch;
    float yaw;
    float fovx;
} camera_3rd_person;

mat4s cam3p_view_mat(camera_3rd_person c3p);

void cam3p_update_held(
        camera_3rd_person *cam,

        bool pitch_up,
        bool pitch_down,
        bool yaw_left,
        bool yaw_right,
        bool zoom_in,
        bool zoom_out,

        float rot_amount,
        float zoom_amount);