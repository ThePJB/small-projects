#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "ggl.h"
#include "pnc.h"
#include "glad.h"
#include "util.h"
#include "mymath.h"

bool handle_input(double dt) {
    // Events
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) return false;
        if (e.type == SDL_KEYDOWN) {
            SDL_Keycode sym = e.key.keysym.sym;
            if (sym == SDLK_ESCAPE) {
                return false;
            }
        }
    }
    return true;
}

void draw(gg_context *g, shader_pgm_id pgm, PNC_Mesh m) {
    glClearColor(0.3, 0.5, 0.7, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //mat4s view = ...

    // just looking at origin
    mat4s view = glms_lookat(
        (vec3s) {0, 0, -1}, 
        (vec3s) {0, 0, 0}, 
        (vec3s) {0, 1, 0}
    );

    float fovx = 90;
    mat4s proj = glms_perspective(
        glm_rad(fovx), 
        (float)g->xres / (float)g->yres, 0.1, 10000
    );
    pnc_draw(m, pgm, view.raw[0], proj.raw[0]);

    SDL_GL_SwapWindow(g->window);
}

void push_cone(PNC_Mesh *m,  vec3s colour, vec3s tip, vec3s bottom_center, float r, int num_sides) {
    float theta = 2*M_PI / num_sides;

    vec3s base_vec = glms_vec3_add(bottom_center, (vec3s) {r, 0, 0});
    
    for (int i = 0; i < num_sides; i++) {
        vec3s rot_base_vec1 = glms_vec3_rotate(base_vec, i * theta, glms_vec3_sub(tip, bottom_center));
        vec3s rot_base_vec2 = glms_vec3_rotate(base_vec, (i+1) * theta, glms_vec3_sub(tip, bottom_center));
        
        vec3s p1 = glms_vec3_add(bottom_center, rot_base_vec1);
        vec3s p2 = glms_vec3_add(bottom_center, rot_base_vec2);

        vec3s normal = normal_from_verts(p1, tip, p2);
        pnc_push_tri(m, 
            (PNC_Vert) {p1, normal, colour},
            (PNC_Vert) {tip, normal, colour},
            (PNC_Vert) {p2, normal, colour}
        );
    }
}

PNC_Mesh make_pine_tree(
        float trunk_height,
        float trunk_radius
    ) {

    const int base_poly = 6;
    const int foliage_poly = 6;

    const float f1_start = 1.0;
    const float f1_end = 0.7;
    const float f1r = 0.2;
    const float f2_start = 0.75;
    const float f2_end = 0.45;
    const float f2r = 0.3;
    const float f3_start = 0.5;
    const float f3_end = 0.2;
    const float f3r = 0.4;

    const vec3s foliage_colour = (vec3s) {0.1,  0.9, 0.1};
    const vec3s trunk_colour = (vec3s) {0.3,  0.3, 0};

    PNC_Mesh m = pnc_new();
    
    push_cone(&m, trunk_colour, (vec3s) {0, trunk_height, 0}, (vec3s) {0, 0, 0}, trunk_radius, base_poly);
    push_cone(&m, foliage_colour, (vec3s) {0, f1_start, 0}, (vec3s) {0, f1_end, 0}, f1r, foliage_poly);
    //push_cone(&m, foliage_colour, (vec3s) {0, f2_start, 0}, (vec3s) {0, f2_end, 0}, f2r, foliage_poly);
    //push_cone(&m, foliage_colour, (vec3s) {0, f3_start, 0}, (vec3s) {0, f3_end, 0}, f3r, foliage_poly);

    return m;
}

int main(int argc, char** argv) {
    uint64_t frame_us = 1000000 / 60;
    gg_context *g = ggl_init("tree thing", 1280, 720);
    shader_id vert = ggl_make_shader(g, slurp("tree.vert"), GL_VERTEX_SHADER);
    shader_id frag = ggl_make_shader(g, slurp("tree.frag"), GL_FRAGMENT_SHADER);
    shader_pgm_id tree_pgm = ggl_make_shader_pgm(g, vert, frag);
    
    // make and upload a mesh
    //PNC_Mesh m = pnc_test_mesh(); // = ...
    PNC_Mesh m = make_pine_tree(1, 0.1);
    pnc_upload(&m);

    pnc_print_summary(m);
    pnc_print_tris(m);

    bool keep_going = true;

    double dt = 0;
    while (keep_going) {
        int64_t tstart = get_us();
        keep_going = handle_input(dt);

        draw(g, tree_pgm, m);

        int64_t tend = get_us();
        int64_t remaining_us = tend - tstart;
        dt = ((double)remaining_us) / 1000000;
        if (remaining_us < frame_us) {
            usleep(remaining_us);
        }
    }

    ggl_teardown(g);
}