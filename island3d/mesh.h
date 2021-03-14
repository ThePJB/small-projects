#pragma once
#include "glad.h"
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

using glm::mat4;

template<typename V>
struct mesh {
    mat4 model = glm::mat4(1);
    
    vao m_vao = 0;
    vbo m_vbo = 0;
    bool uploaded = false;

    vla<V> verts = vla<V>();
    
    // assuming vert destructor gets automatically called?
    void upload() {
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(V) * verts.length, verts.items, GL_STATIC_DRAW);

        V::set_vertex_attributes();
        uploaded = true;
    }

    void draw(dgl_context *gc, shader_pgm_id pgm, mat4 view, mat4 proj) {
        printf("draw, vao %u\n", m_vao);
        if (!uploaded) {
            printf("warning: trying to draw non uploaded mesh\n");
        }

        printf("view:\n%s\n", glm::to_string(view).c_str());
        printf("proj:\n%s\n", glm::to_string(proj).c_str());
        printf("model:\n%s\n", glm::to_string(model).c_str());

        glUseProgram(pgm);
        glUniformMatrix4fv(glGetUniformLocation(pgm, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(pgm, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(pgm, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, verts.length);
    }

    void push(V v) {
        verts.push(v);
    }
};