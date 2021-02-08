#pragma once

#include "grid.h"
#include "gef.h"

typedef struct {
    float food;
    int type;
} predator_prey_tile;

typedef struct {
    grid g;

    float grass_rate;
    float grass_max;

    float pred_hunger_rate;
    float pred_reproduce_above;
    float pred_reproduce_cost;
    float pred_starting_food;
    float starting_pred_chance;

    float prey_reproduce_above;
    float prey_reproduce_cost;
    float prey_starting_food;
    float prey_food_value;
    float starting_prey_chance;
} predator_prey_context;

void *rule_predator_prey_init(int xres, int yres);
void rule_predator_prey_update(void *pp_context);
void rule_predator_prey_draw(gef_context *gc, void *pp_context);
void rule_predator_prey_reset(void *state, int xres, int yres);
bool rule_predator_prey_menu_str(void *state, int index, char *buf);
void rule_predator_prey_menu_succ(void *state, int index, bool up);