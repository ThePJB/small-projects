#include "ggl.h"
#include "pnc.h"
#include "glad.h"

void pnc_upload(PNC_Mesh *m) {
    vbo pnc_vbo;
    vao *m_vao = &(m->vao);
    glGenVertexArrays(1, m_vao);
    glGenBuffers(1, &pnc_vbo);
    glBindVertexArray(m->vao);
    glBindBuffer(GL_ARRAY_BUFFER, pnc_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PNC_Tri) * m->num_tris, m->tris, GL_STATIC_DRAW);
    
    size_t offset = 0;

    // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PNC_Vert), (void*)offset);
    glEnableVertexAttribArray(0);
    offset += 3 * sizeof(float);

    // true is for normalize (normal)
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PNC_Vert), (void*)offset);
    glEnableVertexAttribArray(1);
    offset += 3 * sizeof(float);
    
    // colour
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(PNC_Vert), (void*)offset);
    glEnableVertexAttribArray(2);
    offset += 3 * sizeof(float);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m->uploaded = true;
}

void pnc_draw(PNC_Mesh m, shader_pgm_id pgm, float *view, float *proj) {
    if (m.uploaded == false) {
        printf("warning: tried to draw mesh which hasn't been uploaded\n");
    }

    glUseProgram(pgm);
    glUniformMatrix4fv(glGetUniformLocation(pgm, "view"), 1, GL_FALSE, view);
    glUniformMatrix4fv(glGetUniformLocation(pgm, "proj"), 1, GL_FALSE, proj);
    glBindVertexArray(m.vao);
    glDrawArrays(GL_TRIANGLES, 0, m.num_tris * 3); // handle should probably contain num triangles
}

// todo free from ram
// todo free from vram

PNC_Mesh pnc_new() {
    const size_t default_backing_mesh_size = 10;

    PNC_Mesh m = {0};
    m.tris = calloc(default_backing_mesh_size, sizeof(PNC_Tri));
    m.backing_length = default_backing_mesh_size;
    return m;
}

void pnc_push_tri(PNC_Mesh *m, PNC_Vert v1, PNC_Vert v2, PNC_Vert v3) {
    // problem with realloc, array gets wiped or replaced with random memory or something
    if (m->num_tris >= m->backing_length) {
        m->tris = realloc(m->tris, m->backing_length * 2);
        m->backing_length *= 2;
    }

    m->tris[m->num_tris][0] = v1;
    m->tris[m->num_tris][1] = v2;
    m->tris[m->num_tris][2] = v3;
    m->num_tris++;
}

PNC_Mesh pnc_test_mesh() {
    PNC_Mesh m = pnc_new();

    PNC_Vert v1, v2, v3;
    v1 = (PNC_Vert) {
        (vec3s) {-1, 0, 0},
        (vec3s) {0, 0, -1},
        (vec3s) {1, 0, 0}
    };
    v2 = (PNC_Vert) {
        (vec3s) {0, 1, 0},
        (vec3s) {0, 0, -1},
        (vec3s) {0, 1, 0}
    };
    v3 = (PNC_Vert) {
        (vec3s) {1, 0, 0},
        (vec3s) {0, 0, -1},
        (vec3s) {0, 0, 1}
    };
    pnc_push_tri(&m, v1, v2, v3);
    
    return m;
}

void pnc_print_summary(PNC_Mesh m) {
    printf("Length: %zu\n", m.backing_length);
    printf("Num Tris: %zu\n", m.num_tris);
    printf("Tris ptr: %p\n", m.tris);
    printf("Vao: %u\n", m.vao);
    printf("Uploaded: %s\n", m.uploaded ? "true" : "false");    
}

void pnc_print_tris(PNC_Mesh m) {
    for (int i = 0; i < m.num_tris; i++) {
        printf("tri %d:\n", i);
        for (int j = 0; j < 3; j++) {
            PNC_Vert vert = m.tris[i][j];
            printf("\tvert %d:\n", j);
            printf("\t\tpos: %.2f %.2f %.2f\n", vert.pos.x, vert.pos.y, vert.pos.z);
            printf("\t\tnormal: %.2f %.2f %.2f\n", vert.normal.x, vert.normal.y, vert.normal.z);
            printf("\t\tcolour: %.2f %.2f %.2f\n", vert.colour.x, vert.colour.y, vert.colour.z);
        };
    }
}