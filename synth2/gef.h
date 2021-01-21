#pragma once

#include <stdio.h>
#include <SDL.h>

#define gef_die(G, X) printf("%s %d %s: dying -- %s\n", __FILE__, __LINE__, __func__, X), gef_teardown(G)

typedef struct { 
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *atlas;
    int xres;
    int yres;
    int frame_cap;
    int64_t tstart;

} gef_context;

gef_context gef_init(char *name, int xres, int yres, int frame_cap);
void gef_load_atlas(gef_context *cg, char *path);
void gef_draw_sprite(gef_context *cg, SDL_Rect clip, SDL_Rect to_rect);
void gef_teardown(gef_context *gc);

void gef_start_frame(gef_context *gc);
void gef_end_frame(gef_context *gc);

void gef_clear(gef_context *gc);
void gef_set_colour(gef_context *gc, int r, int g, int b, int a);
void gef_present(gef_context *gc);

