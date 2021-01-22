#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "util.h"
#include "math.h"

int util_rand_ab(int a, int b) {
    return a + rand() % b;
}

uint64_t get_us() { 
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)t.tv_sec * 1000000 + (uint64_t)t.tv_nsec / 1000;
}
SDL_Rect rect_dilate(SDL_Rect r, int amount) {
    return (SDL_Rect) {
        r.x - amount,
        r.y - amount,
        r.w + 2*amount,
        r.h + 2*amount,
    };
}

float dot(float x1, float y1, float x2, float y2) {
    return x1 * x2 + y1 * y2;
}

float distance(float x1, float y1, float x2, float y2) {
    return sqrtf((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

float dist_point_line(float px, float py, float x1, float y1, float x2, float y2) {
    return fabs((x2 - x1) * (y1 - py) - (x1 - px) * (y2 - y1)) / sqrtf((x2 - x1)*(x2 - x1) + (y2 - y1) * (y2 - y1));
}

// no forgiveness if ur off the end of the line stooge
float dist_point_line_seg(float px, float py, float x1, float y1, float x2, float y2) {
    float dist = distance(x1,y1,x2,y2);
    if (dist == 0) {
        return distance(x1,y1,px,py);
    }
    float t = dot(px - x1, py - y1, x2 - x1, y2 - y1) / (dist*dist);
    //if (t < 0 || t > 1) return INFINITY;
    if (t < 0) t = 0;
    if (t > 1) t = 1;

    float proj_x = x1 + t * (x2 - x1);
    float proj_y = y1 + t * (y2 - y1);
    return distance(proj_x, proj_y, px, py);
}