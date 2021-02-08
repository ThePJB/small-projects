#include "predator_prey.h"
#include "util.h"

const int grass = 0;
const int prey = 1;
const int predator = 2;

void *rule_predator_prey_init(int xres, int yres) {
    predator_prey_context *pp_context = calloc(1, sizeof(predator_prey_context));
    pp_context->g = grid_init(sizeof(predator_prey_tile), xres, yres);
    pp_context->starting_pred_chance = 0.01;
    pp_context->starting_prey_chance = 0.01;

    pp_context->grass_rate = 0.005;
    pp_context->grass_max = 1;

    pp_context->pred_hunger_rate = 0.005;
    pp_context->pred_reproduce_above = 1;
    pp_context->pred_reproduce_cost = 0.5;
    pp_context->pred_starting_food = 0.5;

    pp_context->prey_reproduce_above = 1;
    pp_context->prey_reproduce_cost = 1;
    pp_context->prey_starting_food = 0;
    pp_context->prey_food_value = 0.2;

    rule_predator_prey_reset(pp_context, xres, yres);

    return pp_context;
}

int generation = 0;

void rule_predator_prey_update(void *pp_context_vp) {
    predator_prey_context *pp_context = (predator_prey_context*)pp_context_vp;
    grid g = pp_context->g;
    int ngrass = 0;
    int npred = 0;
    int nprey = 0;

    for (int i = 0; i < g.w; i++) {
        for (int j = 0; j < g.h; j++) {
            predator_prey_tile t;
            grid_get(g, &t, i, j);

            if (t.type == grass) {
                ngrass++;
                t.food += pp_context->grass_rate;
                t.food = min(t.food, pp_context->grass_max);
                grid_set(g, &t, i, j);
                continue;
            }

            if (t.type == predator) {
                npred++;
                t.food -= pp_context->pred_hunger_rate;
                grid_set(g, &t, i, j);
                if (t.food <= 0) {
                    t.type = grass;
                    t.food = 0;
                    grid_set(g, &t, i, j);
                    continue;
                }
            }

            if (t.type == prey) {
                nprey++;
            }

            int move_x, move_y;
            while (!grid_index_neighbour8(g, i, j, util_rand_ab(0, 8), &move_x, &move_y));
            predator_prey_tile candidate;
            grid_get(g, &candidate, move_x, move_y);

            if (t.type == prey) {
                if (candidate.type == grass) {
                    t.food += candidate.food;
                    if (t.food > pp_context->prey_reproduce_above) {
                        t.food -= pp_context->prey_reproduce_cost;
                        candidate.type = prey;
                        candidate.food = pp_context->prey_starting_food;
                    } else {
                        candidate.food = 0;
                    }
                    // swap positions of original and candidate
                    grid_set(g, &t, move_x, move_y);
                    grid_set(g, &candidate, i, j);
                }
                // dont do anything if its another prey or if its a predator
            } else if (t.type == predator) {
                if (candidate.type == prey) {
                    t.food += pp_context->prey_food_value;
                    if (t.food > pp_context->pred_reproduce_above) {
                        t.food -= pp_context->pred_reproduce_cost;
                        candidate.type = predator;
                        candidate.food = pp_context->pred_starting_food;
                    } else {
                        candidate.type = grass;
                        candidate.food = 0;
                    }
                    // swap positions of original and candidate
                    grid_set(g, &t, move_x, move_y);
                    grid_set(g, &candidate, i, j);
                } else if (candidate.type == grass) {
                    // if its grass we just swap positions (moving through)
                    grid_set(g, &t, move_x, move_y);
                    grid_set(g, &candidate, i, j);
                }
            }
        }
    }
    printf("generation %d: %d grass, %d prey, %d pred\n", generation++, ngrass, nprey, npred);
}

void rule_predator_prey_draw(gef_context *gc, void *pp_context) {
    grid g = ((predator_prey_context*)pp_context)->g;
    for (int i = 0; i < g.w; i++) {
        for (int j = 0; j < g.h; j++) {
            predator_prey_tile t;
            grid_get(g, &t, i, j);

            if (t.type == grass) {
                int greenness = 100 + t.food * 100;
                gef_put_pixel(gc, 50, greenness, 70, 255, i, j);
            } else if (t.type == prey) {
                gef_put_pixel(gc, 255, 255, 255, 255, i, j);
            } else if (t.type == predator) {
                gef_put_pixel(gc, 255, 100, 100, 255, i, j);
            }
        }
    }
}

void rule_predator_prey_reset(void *state, int xres, int yres) {
    predator_prey_context *pp_context = (predator_prey_context*)state;
    for (int i = 0; i < pp_context->g.w; i++) {
        for (int j = 0; j < pp_context->g.h; j++) {
            predator_prey_tile t;
            float r = rand_floatn(0, 1);
            if (r < pp_context->starting_pred_chance) {
                t.type = predator;
                t.food = pp_context->pred_starting_food;
            } else if (r < pp_context->starting_prey_chance + pp_context->starting_pred_chance) {
                t.type = prey;
                t.food = pp_context->prey_starting_food;
            } else {
                t.type = grass;
                t.food = 0;
            }
            grid_set(pp_context->g, &t, i, j);
        }
    }
}

bool rule_predator_prey_menu_str(void *state, int index, char *buf) {
    predator_prey_context *pp_context = (predator_prey_context*)state;

    if (index == 0) {
        sprintf(buf, "grass rate %.4f", pp_context->grass_rate);
        return true;
    } else if (index == 1) {
        sprintf(buf, "grass max %.4f", pp_context->grass_max);
        return true;
    } else if (index == 2) {
        sprintf(buf, "pred hunger rate %.4f", pp_context->pred_hunger_rate);
        return true;
    } else if (index == 3) {
        sprintf(buf, "pred reproduce threshold %.4f", pp_context->pred_reproduce_above);
        return true;
    } else if (index == 4) {
        sprintf(buf, "pred reproduce cost %.4f", pp_context->pred_reproduce_cost);
        return true;
    } else if (index == 5) {
        sprintf(buf, "pred starting food %.4f", pp_context->pred_starting_food);
        return true;
    } else if (index == 6) {
        sprintf(buf, "starting pred chance %.4f", pp_context->starting_pred_chance);
        return true;
    } else if (index == 7) {
        sprintf(buf, "prey reproduce threshold %.4f", pp_context->prey_reproduce_above);
        return true;
    } else if (index == 8) {
        sprintf(buf, "prey reproduce cost %.4f", pp_context->prey_reproduce_cost);
        return true;
    } else if (index == 9) {
        sprintf(buf, "prey starting food %.4f", pp_context->prey_starting_food);
        return true;
    } else if (index == 10) {
        sprintf(buf, "prey food value %.4f", pp_context->prey_food_value);
        return true;
    } else if (index == 11) {
        sprintf(buf, "starting prey chance %.4f", pp_context->starting_prey_chance);
        return true;
    } else {
        return false;
    }
}

void rule_predator_prey_menu_succ(void *state, int index, bool up) {
    predator_prey_context *pp_context = (predator_prey_context*)state;

    float direction_sign = 1;
    if (!up) {
        direction_sign = -1;
    }

    if (index == 0) {
        pp_context->grass_rate += 0.001 * direction_sign;
    } else if (index == 1) {
        pp_context->grass_max += 0.01 * direction_sign;
    } else if (index == 2) {
        pp_context->pred_hunger_rate += 0.001 * direction_sign;
    } else if (index == 3) {
        pp_context->pred_reproduce_above += 0.001 * direction_sign;
    } else if (index == 4) {
        pp_context->pred_reproduce_cost += 0.001 * direction_sign;
    } else if (index == 5) {
        pp_context->pred_starting_food += 0.001 * direction_sign;
    } else if (index == 6) {
        pp_context->starting_pred_chance += 0.001 * direction_sign;
    } else if (index == 7) {
        pp_context->prey_reproduce_above += 0.001 * direction_sign;
    } else if (index == 8) {
        pp_context->prey_reproduce_cost += 0.001 * direction_sign;
    } else if (index == 9) {
        pp_context->prey_starting_food += 0.001 * direction_sign;
    } else if (index == 10) {
        pp_context->prey_food_value += 0.001 * direction_sign;
    } else if (index == 11) {
        pp_context->starting_prey_chance += 0.001 * direction_sign;
    }
}