#pragma once

#include <stdint.h>
#include <SDL.h>

int util_rand_ab(int a, int b);
uint64_t get_us();


#define len(X) (sizeof(X) / sizeof(X[0]))

#define min(A,B) (A < B ? A : B)
#define max(A,B) (A > B ? A : B)

SDL_Rect rect_dilate(SDL_Rect r, int amount);
float dist_point_line(float px, float py, float x1, float y1, float x2, float y2);
float dist_point_line_seg(float px, float py, float x1, float y1, float x2, float y2);