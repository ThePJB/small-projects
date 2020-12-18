#pragma once

#include <stdio.h>

#define gef_die(X) printf("%s %d %s: dying -- %s\n", __FILE__, __LINE__, __func__, X), gef_teardown()

void gef_init();
void gef_set_name(char *name);
void gef_set_res(int x, int y);
void gef_load_atlas(char *path);
void gef_draw_sprite(SDL_Rect clip, SDL_Rect to_rect);
void gef_teardown();

void gef_set_frame_cap(int fps);
void gef_start_frame();
void gef_end_frame();

int gef_frame_cap();
int gef_xres();
int gef_yres();

void gef_clear();
void gef_set_colour(int r, int g, int b, int a);
void gef_present();

