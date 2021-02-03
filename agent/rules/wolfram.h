#pragma once

#include "grid.h"
#include "gef.h"

typedef struct {
    int generation;
    int curr_buffer;
    uint8_t rule;
    grid g[2];
} wolfram_context;

void *rule_wolfram_init(int xres, int yres);
void rule_wolfram_reset_grid(void *state, int xres, int yres);
void *rule_wolfram_init_random(int xres, int yres);
void rule_wolfram_reset_grid_random(void *state, int xres, int yres);
void rule_wolfram_update(void *state);
void rule_wolfram_draw(gef_context *gc, void *state);

bool rule_wolfram_menu(void *state, int index, char *buf);
void rule_wolfram_succ(void *state, int index, bool up);