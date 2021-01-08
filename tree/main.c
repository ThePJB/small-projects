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
#include "text.h"
#include "menu.h"

typedef struct {
    gg_context *g;

    shader_pgm_id tree_program;
    PNC_Mesh current_tree;

    shader_pgm_id line_program;
    line_mesh axis;

    camera_3rd_person cam;

    int current_tree_number;

    bool keep_going;

    l_tree_params ltp;

    menu tree_settings_menu;
} application;

PNC_Mesh make_l_tree(int seed, l_tree_params ltp) {
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
    l_tree(seed, &tp, base, ltp, ltp.tree_t);
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
            } else if (sym == SDLK_u) {
                retree = true;
                app->current_tree_number -= 1;
                printf("tree %d\n", app->current_tree_number);
            } else if (sym == SDLK_i) {
                retree = true;
                app->current_tree_number += 1;
                printf("tree %d\n", app->current_tree_number);


            } else if (sym == SDLK_h) {
                menu_update(&app->tree_settings_menu, MD_LEFT);
                retree = true;
            } else if (sym == SDLK_l) {
                menu_update(&app->tree_settings_menu, MD_RIGHT);
                retree = true;
            } else if (sym == SDLK_j) {
                menu_update(&app->tree_settings_menu, MD_DOWN);
                retree = true;
            } else if (sym == SDLK_k) {
                menu_update(&app->tree_settings_menu, MD_UP);
                retree = true;
            }
        }
    }

    if (retree) {
        pnc_free_ram(app->current_tree);
        pnc_free_vram(app->current_tree);
        app->current_tree = make_l_tree(app->current_tree_number, app->ltp);
        printf("tree %d\n", app->current_tree_number);
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

    menu_draw(&app->tree_settings_menu, 10, 100);

    line_draw(app->axis, app->line_program, view.raw[0], proj.raw[0]);
    pnc_draw(app->current_tree, app->tree_program, view.raw[0], proj.raw[0]);



    SDL_GL_SwapWindow(app->g->window);
}

int main(int argc, char** argv) {
    uint64_t frame_us = 1000000 / 60;
    gg_context *g = ggl_init("tree thing", 1920, 1080);
    text_init(g);

    shader_id vert = ggl_make_shader(g, slurp("shaders/tree.vert"), GL_VERTEX_SHADER);
    shader_id frag = ggl_make_shader(g, slurp("shaders/tree.frag"), GL_FRAGMENT_SHADER);
    shader_pgm_id tree_pgm = ggl_make_shader_pgm(g, vert, frag);
    
    shader_id line_vert = ggl_make_shader(g, slurp("shaders/line.vert"), GL_VERTEX_SHADER);
    shader_id line_frag = ggl_make_shader(g, slurp("shaders/line.frag"), GL_FRAGMENT_SHADER);
    shader_pgm_id line_pgm = ggl_make_shader_pgm(g, line_vert, line_frag);


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

    menu menu = menu_new();

    application app = (application) {
        .axis = axis,
        .cam = c3p,
        .g = g,
        .line_program = line_pgm,
        .tree_program = tree_pgm,
        .current_tree_number = 0,
        .keep_going = true,

        .ltp = (l_tree_params) {
            .tree_t = 1.0,
            .branch_keep_chance = 0.5,
            .d_const = 0.4,
            .d_linear = 0.1,
            .up_tendency = 0.1,
            .branch_range = 0.6,
            .t_increment = 0.15,
            .thickness = 0.2,
            .trunk_percentage = 0.6,
        },

        .tree_settings_menu = menu,
    };

    app.current_tree = make_l_tree(0, app.ltp);


    menu_push(&app.tree_settings_menu, "t", &app.ltp.tree_t, 0, 4, 0.1);
    menu_push(&app.tree_settings_menu, "thickness", &app.ltp.thickness, 0, 1, 0.02);
    menu_push(&app.tree_settings_menu, "trunk percent", &app.ltp.trunk_percentage, 0, 1, 0.05);
    menu_push(&app.tree_settings_menu, "branch %", &app.ltp.branch_keep_chance, 0, 1, 0.05);
    menu_push(&app.tree_settings_menu, "upness", &app.ltp.up_tendency, 0, 1, 0.05);
    menu_push(&app.tree_settings_menu, "d const", &app.ltp.d_const, 0, 1, 0.05);
    menu_push(&app.tree_settings_menu, "d linear", &app.ltp.d_linear, 0, 1, 0.05);
    menu_push(&app.tree_settings_menu, "branch dev range", &app.ltp.branch_range, 0, 1.5, 0.05);
    menu_push(&app.tree_settings_menu, "t inc", &app.ltp.t_increment, 0.01, 0.5, 0.01);
    

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