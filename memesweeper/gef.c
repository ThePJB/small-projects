#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <unistd.h>

#include "util.h"
#include "stdint.h"
#include "gef.h"

typedef struct { 
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *atlas;
    int xres;
    int yres;
    int frame_cap;
    int64_t tstart;

} gef;

gef gef_context = {0};

void gef_set_name(char *name) {
    SDL_SetWindowTitle(gef_context.window, name);
}

void gef_set_res(int x, int y) {
    gef_context.xres = x;
    gef_context.yres = y;
    SDL_SetWindowSize(gef_context.window, x, y);
}

int gef_xres() {
    return gef_context.xres;
}

int gef_yres() {
    return gef_context.xres;
}

void gef_init() {
    printf("initializing graphics...\n");
    gef *g = calloc(1, sizeof(gef));

    gef_context.xres = 640;
    gef_context.yres = 480;
    gef_context.frame_cap = 60;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) gef_die("couldn't init sdl");

    gef_context.window = SDL_CreateWindow("", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        gef_context.xres, 
        gef_context.yres,
        SDL_WINDOW_SHOWN);

    if (gef_context.window == NULL) gef_die("couldn't create window");

    gef_context.renderer = SDL_CreateRenderer(gef_context.window, -1, SDL_RENDERER_ACCELERATED);
    if (gef_context.renderer == NULL) gef_die("couldn't create renderer");

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) gef_die("couldn't init SDL_img");
}

void gef_load_atlas(char *path) {
    SDL_Surface* loaded_surface = IMG_Load(path);
    gef_context.atlas = SDL_CreateTextureFromSurface(gef_context.renderer, loaded_surface);
    if (gef_context.atlas == NULL) gef_die("couldn't create texture");
    SDL_FreeSurface(loaded_surface);
}
void gef_draw_sprite(SDL_Rect clip, SDL_Rect to_rect) {
    SDL_RenderCopy(gef_context.renderer, gef_context.atlas, &clip, &to_rect);
}

void gef_clear() {
    SDL_RenderClear(gef_context.renderer);
}

void gef_set_colour(int r, int g, int b, int a) {
    SDL_SetRenderDrawColor(gef_context.renderer, 0, 0, 0, 255);
}

void gef_present() {
    SDL_RenderPresent(gef_context.renderer);
}

void gef_teardown() {
    SDL_DestroyRenderer(gef_context.renderer);
    SDL_DestroyWindow(gef_context.window);
    IMG_Quit();
    SDL_Quit();
}

void gef_set_frame_cap(int fps) {
    gef_context.frame_cap = fps;
}

void gef_start_frame() {
    gef_context.tstart = get_us();
}
void gef_end_frame() {
    int64_t tnow = get_us();
    int64_t frame_us = 1000000 / gef_context.frame_cap;
    int64_t dt = tnow - gef_context.tstart;
    if (dt < frame_us) {
        int64_t remaining_us = frame_us - dt;
        usleep(remaining_us);
    }
};

int gef_frame_cap();
int gef_xres();
int gef_yres();
