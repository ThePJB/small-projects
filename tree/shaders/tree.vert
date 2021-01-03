#version 330 core

// there was some nice thing like namespacing or something

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 colour;

out vec3 vert_colour;

uniform mat4 view;
uniform mat4 proj;

void main() {
    vec3 sun = vec3(1, 1, 0);
    gl_Position = proj * view * vec4(pos, 1);

    float diffuse = (dot(normal, sun) + 1) / 2;

    const float Kd = 0.5;
    const float Ka = 0.5;

    vert_colour = colour*Ka + colour*diffuse*Kd;
}
