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
} traffic_jam_context;

void *rule_traffic_jam_init(int xres, int yres);
void rule_traffic_jam_update(void *pp_context);
void rule_traffic_jam_draw(gef_context *gc, void *pp_context);