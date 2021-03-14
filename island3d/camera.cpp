#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

using glm::vec3;
using glm::mat3;
using glm::mat4;

void camera::look(int x, int y) {
    yaw += sensitivity * x;
    pitch -= sensitivity * y;
    pitch = dm_clamp(-89, pitch, 89);

    front = glm::normalize(vec3(
        cosf(DEG_TO_RAD*yaw) * cosf(DEG_TO_RAD*pitch),
        sinf(DEG_TO_RAD*pitch),
        sin(DEG_TO_RAD*yaw) * cos(DEG_TO_RAD*pitch)
    ));
}

void camera::move(vec3 dir, float distance) {
    // just a yaw mat
    // sorry about the plus 90 lmao
    // maybe it no need starting yaw is 90? idk
    // i think i may have swapped yaw and pitch as well
    // anyway itll do for now and also i kinda like bomb camera hey
    // this is filthy cause its a pitch mat with yaw+90degrees in it
    mat3 rot_mat = mat3(
        cosf(DEG_TO_RAD*(yaw+90)), 0, sinf(DEG_TO_RAD*(yaw+90)),
        0, 1, 0,
        -sinf(DEG_TO_RAD*(yaw+90)), 0, cosf(DEG_TO_RAD*(yaw+90))
    );
    pos = pos + ((rot_mat * dir) * distance);   
    // this is wrong rn but its g
    // its like transform it by the rotatey part of the view matrix
}

mat3 camera::rotation() {
    // = yaw matrix * pitch matrix?
    mat3 yaw_mat = mat3(
        cosf(DEG_TO_RAD*yaw), -sinf(DEG_TO_RAD*yaw), 0,
        sinf(DEG_TO_RAD*yaw), cosf(DEG_TO_RAD*yaw), 0,
        0, 0, 1
    );

    mat3 pitch_mat = mat3(
        cosf(DEG_TO_RAD*pitch), 0, sinf(DEG_TO_RAD*pitch),
        0, 1, 0,
        -sinf(DEG_TO_RAD*pitch), 0, cosf(DEG_TO_RAD*pitch)
    );

    return pitch_mat*yaw_mat;
}

mat4 camera::view() {
    return glm::lookAt(
        pos,
        pos + front,
        up
    );
};