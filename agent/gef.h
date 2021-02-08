#pragma once

#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>


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

typedef struct {
    TTF_Font *gfont;
} font_handle;

typedef struct {
    SDL_Texture *texture;
    SDL_Texture *texture_shadow;
    int w;
    int h;
} text_handle;

gef_context gef_init(char *name, int xres, int yres, int frame_cap);
void gef_load_atlas(gef_context *cg, char *path);
void gef_draw_sprite(gef_context *cg, SDL_Rect clip, SDL_Rect to_rect);
void gef_teardown(gef_context *gc);

void gef_start_frame(gef_context *gc);
void gef_end_frame(gef_context *gc);

void gef_clear(gef_context *gc);
void gef_set_colour(gef_context *gc, int r, int g, int b, int a);
void gef_put_pixel(gef_context *gc, int r, int g, int b, int a, int x, int y);
void gef_present(gef_context *gc);

font_handle gef_load_font(char *path, int size);
text_handle gef_make_text(gef_context *gc, font_handle f, char *text, int r, int g, int b);
void gef_draw_text(gef_context *gc, text_handle text, int x, int y);
void gef_destroy_text(text_handle text);