#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "ggl.h"
#include "pnc.h"
#include "glad.h"
#include "util.h"
#include "mymath.h"
#include "proc_tree.h"

typedef struct {
    vec3s target;
    vec3s up;
    float distance;
    float pitch;
    float yaw;
    float fovx;
} camera_3rd_person;

mat4s view_mat(camera_3rd_person c3p) {
    vec3s u = (vec3s) {0,0,1};
    
    u = glms_vec3_rotate(u, c3p.yaw, c3p.up);

    // pitch needs to rotate against right vector
    // right is forward x up
    vec3s forward = glms_vec3_sub(c3p.target, u);
    vec3s right = glms_vec3_cross(forward, c3p.up);

    u = glms_vec3_rotate(u, c3p.pitch, glms_vec3_normalize(right));
    
    
    vec3s pos = glms_vec3_sub(c3p.target, glms_vec3_scale(u, c3p.distance));
    //vec3s up = glms_vec3_rotate(c3p.up, c3p.pitch, (vec3s) {0,0,1});
    return glms_lookat(pos, c3p.target, glms_vec3_cross(u, right));
}

typedef enum {
    IA_NONE,
    IA_QUIT,
    IA_RESET,
} input_action;

input_action handle_input(double dt, camera_3rd_person *c3p) {
    const float rot_speed = 3;
    const float zoom_speed = 3;

    // Events
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) return IA_QUIT;
        if (e.type == SDL_KEYDOWN) {
            SDL_Keycode sym = e.key.keysym.sym;
            if (sym == SDLK_ESCAPE) {
                return IA_QUIT;
            } else if (sym == SDLK_r) {
                return IA_RESET;
            }
        }
    }

    const uint8_t *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_W]) {
        c3p->pitch += rot_speed*dt;
    }
    if (state[SDL_SCANCODE_S]) {
        c3p->pitch -= rot_speed*dt;
    }
    if (state[SDL_SCANCODE_A]) {
        c3p->yaw += rot_speed*dt;
    }
    if (state[SDL_SCANCODE_D]) {
        c3p->yaw -= rot_speed*dt;
    }
    if (state[SDL_SCANCODE_Q]) {
        c3p->distance += zoom_speed*dt;
    }
    if (state[SDL_SCANCODE_E]) {
        c3p->distance -= zoom_speed*dt;
    }


    return IA_NONE;
}

void draw(gg_context *g, shader_pgm_id pgm, PNC_Mesh m, camera_3rd_person c3p) {
    glClearColor(0.3, 0.5, 0.7, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //mat4s view = ...

    // just looking at origin

    mat4s view = view_mat(c3p);
    /*
        glms_lookat(
        (vec3s) {0, 1, -1}, 
        (vec3s) {0, 0, 0}, 
        (vec3s) {0, 1, 0}
    );
    */

    float fovx = 90;
    mat4s proj = glms_perspective(
        glm_rad(fovx), 
        (float)g->xres / (float)g->yres, 0.1, 10000
    );
    pnc_draw(m, pgm, view.raw[0], proj.raw[0]);

    SDL_GL_SwapWindow(g->window);
}



PNC_Mesh do_mainmesh() {
    vec3s green = (vec3s) {0.2, 0.6, 0.1};
    vec3s brown = (vec3s) {0.4, 0.4, 0.1};

    const float rot_mag = 0.5;

    PNC_Mesh m = pnc_new();
    tree_parameters tp = tree_start((vec3s){0,0,0}, 8, green, brown);
    tp.foliage_type = FT_CONE;
    trunk_cross_section base = (trunk_cross_section) {
        .position = {0, 0, 0},
        .axis = glms_vec3_normalize((vec3s) {rand_floatn(-rot_mag, rot_mag), 1, rand_floatn(-rot_mag, rot_mag)}),
        .radius = rand_floatn(0.1, 0.5),
    };
    //tree_continue(&tp, base, 0);
    l_tree(&tp, base, 0);
    tree_push_to_mesh(&m, tp);
    pnc_upload(&m);
    return m;
}

PNC_Mesh test_push_ellipsoid() {
    PNC_Mesh m = pnc_new();

    pnc_push_ellipsoid(&m, (vec3s) {1, 0, 0}, (vec3s) {0,0.5,0}, (vec3s) {0, 1, 0}, 1, 0.5, 15, 15);

    pnc_upload(&m);
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
    //PNC_Mesh m = make_pine_tree(1, 0.1, 0, 0.05, 6, 0.2, 0.3, 0.15, 0.1);
    //PNC_Mesh m = make_pine_tree(1, 0.06, 0, 0.05, 3, 0.2, 0.6, 0.15, 0.07);
    //PNC_Mesh m = make_pine_tree(2, 0.4, 0, 0.05, 6, 0.25, 0.3, 0.3, 0.15);
    
    //PNC_Mesh m = example_branch_tree();
    //PNC_Mesh m = example_bottle_tree();

    //PNC_Mesh m = do_mainmesh();
    PNC_Mesh m = test_push_ellipsoid();


    //pnc_print_summary(m);
    //pnc_print_tris(m);

    camera_3rd_person c3p = {
        .distance = 2,
        .fovx = 90,
        .pitch = 0,
        .yaw = 0,
        .target = (vec3s) {0,0.5,0},
        .up = (vec3s) {0, 1, 0}
    };

    bool keep_going = true;

    double dt = 0;
    while (keep_going) {
        int64_t tstart = get_us();
        input_action ia = handle_input(dt, &c3p);
        keep_going = (ia != IA_QUIT);
        if (ia == IA_RESET) {
            m = do_mainmesh();
            // todo - mesh & tree thing cleanup
        }

        draw(g, tree_pgm, m, c3p);

        int64_t tend = get_us();
        int64_t remaining_us = tend - tstart;
        dt = ((double)remaining_us) / 1000000;
        if (remaining_us < frame_us) {
            usleep(remaining_us);
        }
    }

    ggl_teardown(g);
}