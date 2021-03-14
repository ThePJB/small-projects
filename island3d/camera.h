#pragma once

#include <glm/glm.hpp>
#include "dankmath.h"

struct camera {
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 up = glm::vec3(0, 1, 0);
    float pitch;
    float yaw = -90;

    float sensitivity = 0.05;

    camera(glm::vec3 start_pos, glm::vec3 look_at) {
        pos = start_pos;
        front = glm::normalize(look_at - start_pos);
    }
    void look(int x, int y);
    void move(glm::vec3 dir, float distance);
    glm::mat3 rotation();
    glm::mat4 view();
};
