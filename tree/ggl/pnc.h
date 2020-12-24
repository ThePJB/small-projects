#pragma once

#include "ggl.h"
#include <cglm/struct.h>

typedef struct {
    vec3s pos;
    vec3s normal;
    vec3s colour;
} PNC_Vert;

typedef PNC_Vert PNC_Tri[3];

typedef struct {
    size_t backing_length;
    size_t num_tris;
    PNC_Tri *tris;
    
    vao vao;
    bool uploaded;

} PNC_Mesh;

PNC_Mesh pnc_new();
void pnc_push_tri(PNC_Mesh *m, PNC_Vert v1, PNC_Vert v2, PNC_Vert v3);
void pnc_upload(PNC_Mesh *m);
void pnc_draw(PNC_Mesh m, shader_pgm_id pgm, float *view, float *proj);

void pnc_print_summary(PNC_Mesh m);
void pnc_print_tris(PNC_Mesh m);

PNC_Mesh pnc_test_mesh();