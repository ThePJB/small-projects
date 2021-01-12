#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <unistd.h>

#include "util.h"
#include "stdint.h"
#include "gef.h"



gef_context gef_init(char *name, int xres, int yres, int frame_cap) {
    printf("initializing graphics...\n");
    gef_context gc;

    gc.xres = xres;
    gc.yres = yres;
    gc.frame_cap = frame_cap;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) gef_die(&gc, "couldn't init sdl");

    gc.window = SDL_CreateWindow(name, 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        gc.xres, 
        gc.yres,
        SDL_WINDOW_SHOWN);

    if (gc.window == NULL) gef_die(&gc, "couldn't create window");

    gc.renderer = SDL_CreateRenderer(gc.window, -1, SDL_RENDERER_ACCELERATED);
    if (gc.renderer == NULL) gef_die(&gc, "couldn't create renderer");

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) gef_die(&gc, "couldn't init SDL_img");

    return gc;
}

void gef_load_atlas(gef_context *gc, char *path) {
    SDL_Surface* loaded_surface = IMG_Load(path);
    gc->atlas = SDL_CreateTextureFromSurface(gc->renderer, loaded_surface);
    if (gc->atlas == NULL) gef_die(gc, "couldn't create texture");
    SDL_FreeSurface(loaded_surface);
}
void gef_draw_sprite(gef_context *gc, SDL_Rect clip, SDL_Rect to_rect) {
    SDL_RenderCopy(gc->renderer, gc->atlas, &clip, &to_rect);
}

void gef_clear(gef_context *gc) {
    SDL_RenderClear(gc->renderer);
}

void gef_set_colour(gef_context *gc, int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(gc->renderer, r, g, b, 255);
}

void gef_put_pixel(gef_context *gc, int r, int g, int b, int a, int x, int y) {
    SDL_SetRenderDrawColor(gc->renderer, r, g, b, 255);
    SDL_RenderDrawPoint(gc->renderer, x, y);
}

void gef_present(gef_context *gc) {
    SDL_RenderPresent(gc->renderer);
}

void gef_teardown(gef_context *gc) {
    SDL_DestroyRenderer(gc->renderer);
    SDL_DestroyWindow(gc->window);
    IMG_Quit();
    SDL_Quit();
}

void gef_start_frame(gef_context *gc) {
    gc->tstart = get_us();
}
void gef_end_frame(gef_context *gc) {
    int64_t tnow = get_us();
    int64_t frame_us = 1000000 / gc->frame_cap;
    int64_t dt = tnow - gc->tstart;
    if (dt < frame_us) {
        int64_t remaining_us = frame_us - dt;
        usleep(remaining_us);
    }
};

