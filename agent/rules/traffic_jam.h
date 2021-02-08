#pragma once

#include "grid.h"
#include "gef.h"

typedef struct {
    int type;
} traffic_jam_tile;

typedef struct {
    int generation;
    int curr_buffer;
    grid g[2];
    float starting_red_chance;
    float starting_blue_chance;
    bool random_creation;
} traffic_jam_context;

void *rule_traffic_jam_init(int xres, int yres);
void rule_traffic_jam_update(void *pp_context);
void rule_traffic_jam_draw(gef_context *gc, void *pp_context);
void rule_traffic_jam_reset(void *state, int xres, int yres);
bool rule_traffic_jam_menu_str(void *state, int index, char *buf);
void rule_traffic_jam_menu_succ(void *state, int index, bool up);
