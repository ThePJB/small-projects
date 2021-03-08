#pragma once

#include "gef.h"
#include "grid.h"
#include <stdbool.h>

typedef enum {
    T_GRASS,
    T_JUNGLE,
    T_BEACH,
    T_MOUNTAIN,
    T_WATER,
    NUM_TILES,
} tile;

typedef struct {
    gef_context gc;
    grid level;
    bool keep_going;
} application;

void application_init(application *app, int xres, int yres);
void application_update(application *app, double dt);
void application_draw(application *app, double dt);
void application_handle_input(application *app);