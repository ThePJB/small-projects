#pragma once

#include "grid.h"
#include "gef.h"

typedef struct {
    float food;
    int type;
} predator_prey_tile;

typedef struct {
    grid g;
} predator_prey_context;

void *rule_predator_prey_init(int xres, int yres);
void rule_predator_prey_update(void *pp_context);
void rule_predator_prey_draw(gef_context *gc, void *pp_context);