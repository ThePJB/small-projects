#pragma once

#include <SDL.h>
#include "glad.h"

typedef unsigned int vao;
typedef unsigned int vbo;
typedef unsigned int shader_pgm_id;
typedef unsigned int shader_id;

struct dgl_context {
    SDL_Window *window;
    SDL_GLContext gl;
    int xres;
    int yres;

    dgl_context(int starting_xres, int starting_yres);
    ~dgl_context();
    shader_id make_shader(char *program, unsigned int type);
    shader_pgm_id make_shader_pgm(shader_id vertex_shader, shader_id fragment_shader);
    void check_errors();
};