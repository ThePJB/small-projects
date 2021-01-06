#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "ggl.h"
#include "pnc.h"
#include "line.h"
#include "glad.h"
#include "util.h"
#include "mymath.h"
#include "proc_tree.h"
#include "camera3p.h"


typedef struct {
    gg_context *g;

    shader_pgm_id tree_program;
    PNC_Mesh current_tree;

    shader_pgm_id line_program;
    line_mesh axis;

    camera_3rd_person cam;

    int current_tree_number;
    float tree_t;

    bool keep_going;
} application;

PNC_Mesh do_mainmesh(int seed, float start_t) {
    vec3s green = (vec3s) {0.2, 0.6, 0.1};
    vec3s brown = (vec3s) {0.4, 0.4, 0.1};

    const float rot_mag = 0.5;

    PNC_Mesh m = pnc_new();
    tree_parameters tp = tree_start((vec3s){0,0,0}, 4, green, brown);
    tp.foliage_type = FT_ELLIPSOID;
    trunk_cross_section base = (trunk_cross_section) {
        .position = {0, 0, 0},
        .axis = glms_vec3_normalize((vec3s) {
            hash_floatn(seed+48957, -rot_mag, rot_mag),
            1,
            hash_floatn(seed+986123, -rot_mag, rot_mag)}),
        .radius = hash_floatn(seed+9871234, 0.1, 0.5),
    };
    //tree_continue(&tp, base, 0);
    l_tree(seed, &tp, base, start_t);
    tree_push_to_mesh(&m, tp);
    pnc_upload(&m);
    printf("Triangles: %zu\n", m.num_tris);
    return m;
}

void handle_input(double dt, application *app) {
    const float rot_speed = 3;
    const float zoom_speed = 3;

    bool retree = false;

    // Events
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            app->keep_going = false;
            return;
        }
        if (e.type == SDL_KEYDOWN) {
            SDL_Keycode sym = e.key.keysym.sym;
            if (sym == SDLK_ESCAPE) {
                app->keep_going = false;
                return;
            } else if (sym == SDLK_h) {
                retree = true;
                app->current_tree_number -= 1;
            } else if (sym == SDLK_l) {
                retree = true;
                app->current_tree_number += 1;
                printf("tree %d\n", app->current_tree_number);
            } else if (sym == SDLK_j) {
                retree = true;
                app->tree_t -= 0.1;
            } else if (sym == SDLK_k) {
                retree = true;
                app->tree_t += 0.1;
            }
        }
    }

    if (retree) {
        pnc_free_ram(app->current_tree);
        pnc_free_vram(app->current_tree);
        app->current_tree = do_mainmesh(app->current_tree_number, app->tree_t);
        printf("tree %d t: %.1f\n", app->current_tree_number, app->tree_t);

    }

    const uint8_t *state = SDL_GetKeyboardState(NULL);

    cam3p_update_held(&app->cam,
        state[SDL_SCANCODE_W],
        state[SDL_SCANCODE_S],
        state[SDL_SCANCODE_A],
        state[SDL_SCANCODE_D],
        state[SDL_SCANCODE_Q],
        state[SDL_SCANCODE_E],
        rot_speed * dt,
        zoom_speed * dt
    );
}

void draw(application *app) {
    glClearColor(0.3, 0.5, 0.7, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4s view = cam3p_view_mat(app->cam);

    float fovx = 90;
    mat4s proj = glms_perspective(
        glm_rad(fovx), 
        (float)app->g->xres / (float)app->g->yres, 0.1, 10000
    );
    line_draw(app->axis, app->line_program, view.raw[0], proj.raw[0]);
    pnc_draw(app->current_tree, app->tree_program, view.raw[0], proj.raw[0]);

    SDL_GL_SwapWindow(app->g->window);
}



PNC_Mesh test_push_ellipsoid() {
    PNC_Mesh m = pnc_new();

    const int pc = 100;

    pnc_push_ellipsoid(&m, (vec3s) {1, 0, 0}, (vec3s) {0,0,0}, (vec3s) {0, 1, 0}, 1, 0.5, pc, pc);
    pnc_push_ellipsoid(&m, (vec3s) {0, 1, 0}, (vec3s) {-4,0,0}, (vec3s) {0, 1, 0}, 2, 1, pc, pc);
    pnc_push_ellipsoid(&m, (vec3s) {0, 0, 1}, (vec3s) {+4,0,0}, (vec3s) {0, 1, 0}, 2, 2, pc, pc);

    pnc_upload(&m);
    return m;
}

int main(int argc, char** argv) {
    uint64_t frame_us = 1000000 / 60;
    gg_context *g = ggl_init("tree thing", 1280, 720);
    shader_id vert = ggl_make_shader(g, slurp("shaders/tree.vert"), GL_VERTEX_SHADER);
    shader_id frag = ggl_make_shader(g, slurp("shaders/tree.frag"), GL_FRAGMENT_SHADER);
    shader_pgm_id tree_pgm = ggl_make_shader_pgm(g, vert, frag);
    
    shader_id line_vert = ggl_make_shader(g, slurp("shaders/line.vert"), GL_VERTEX_SHADER);
    shader_id line_frag = ggl_make_shader(g, slurp("shaders/line.frag"), GL_FRAGMENT_SHADER);
    shader_pgm_id line_pgm = ggl_make_shader_pgm(g, line_vert, line_frag);

    PNC_Mesh m = do_mainmesh(0, 1);

    line_mesh axis = line_new();
    line_push(&axis, (vec3s) {-1024, 0, 0}, (vec3s) {1024, 0, 0});
    line_push(&axis, (vec3s) {0, -1024, 0}, (vec3s) {0, 1024, 0});
    line_push(&axis, (vec3s) {0, 0, -1024}, (vec3s) {0, 0, 1024});
    line_upload(&axis);

    camera_3rd_person c3p = {
        .distance = 2,
        .fovx = 90,
        .pitch = 0,
        .yaw = 0,
        .target = (vec3s) {0,0,0},
        .up = (vec3s) {0, 1, 0}
    };

    application app = (application) {
        .axis = axis,
        .cam = c3p,
        .current_tree = m,
        .g = g,
        .line_program = line_pgm,
        .tree_program = tree_pgm,
        .current_tree_number = 0,
        .tree_t = 1,
        .keep_going = true,
    };

    double dt = 0;
    while (app.keep_going) {
        int64_t tstart = get_us();

        handle_input(dt, &app);
        draw(&app);

        int64_t tend = get_us();
        int64_t remaining_us = tend - tstart;
        dt = ((double)remaining_us) / 1000000;
        if (remaining_us < frame_us) {
            usleep(remaining_us);
        }
    }

    ggl_teardown(g);
}