#pragma once

#include "grid.h"
#include "gef.h"


void rule_predator_prey_init(grid g);
void rule_predator_prey_update(grid g);
void rule_predator_prey_draw(gef_context *gc, grid g);