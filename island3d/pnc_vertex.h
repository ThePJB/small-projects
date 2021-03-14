#pragma once
#include "glad.h"
#include <glm/glm.hpp>
#include <stdio.h>

using glm::vec3;

struct pnc_vertex {
    vec3 pos;
    vec3 normal;
    vec3 colour;

    pnc_vertex(vec3 p, vec3 n, vec3 c) {
        pos = p;
        normal = n;
        colour = c;
    }

    static void set_vertex_attributes() {
        printf("setting vertex attributes\n");
        size_t size = 3 * 3 * sizeof(float);
        // sizeof(pnc_vertex)
        size_t offset = 0;

        // pos
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(pnc_vertex), (void*)offset);
        glEnableVertexAttribArray(0);
        offset += 3 * sizeof(float);

        // true is for normalize (normal)
        // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(pnc_vertex), (void*)offset);
        glEnableVertexAttribArray(1);
        offset += 3 * sizeof(float);
        
        // colour
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(pnc_vertex), (void*)offset);
        glEnableVertexAttribArray(2);
        offset += 3 * sizeof(float);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};