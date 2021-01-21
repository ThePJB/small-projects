#pragma once

#include "grid.h"
#include "entity.h"

typedef struct {
    float food;
    int type;
} tile;

/*
typedef struct {
    entity *entities;
    int num_entities;
    int backing_array_size;

    grid terrain;
} world;

world world_init(int xres, int yres);
void world_add_entity(world *w, entity e);
void world_step(world *w, void (*rule)(world*, int));
void world_draw(world *w, gef_context *gc);

void world_rule_city(world *w, int ent_idx);
void world_rule_predator_prey(world *w, int ent_idx);
*/