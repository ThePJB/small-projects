#include "predator_prey.h"
#include "util.h"
#include "world.h"

const int grass = 0;
const int prey = 1;
const int predator = 2;

const float grass_rate = 0.005;
const float grass_max = 1;

const float pred_hunger_rate = 0.005;
const float pred_reproduce_above = 1;
const float pred_reproduce_cost = 0.5;
const float pred_starting_food = 0.5;
const float starting_pred_chance = 0.01;

const float prey_reproduce_above = 1;
const float prey_reproduce_cost = 1;
const float prey_starting_food = 0;
const float prey_food_value = 0.2;
const float starting_prey_chance = 0.01;

void rule_predator_prey_init(grid g) {
    for (int i = 0; i < g.w; i++) {
        for (int j = 0; j < g.h; j++) {
            tile t;
            float r = rand_floatn(0, 1);
            if (r < starting_pred_chance) {
                t.type = predator;
                t.food = pred_starting_food;
            } else if (r < starting_prey_chance + starting_pred_chance) {
                t.type = prey;
                t.food = prey_starting_food;
            } else {
                t.type = grass;
                t.food = 0;
            }
            grid_set(g, &t, i, j);
        }
    }
}

int generation = 0;

void rule_predator_prey_update(grid g) {
    int ngrass = 0;
    int npred = 0;
    int nprey = 0;

    for (int i = 0; i < g.w; i++) {
        for (int j = 0; j < g.h; j++) {
            tile t;
            grid_get(g, &t, i, j);

            if (t.type == grass) {
                ngrass++;
                t.food += grass_rate;
                t.food = min(t.food, grass_max);
                grid_set(g, &t, i, j);
                continue;
            }

            if (t.type == predator) {
                npred++;
                t.food -= pred_hunger_rate;
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
            tile candidate;
            grid_get(g, &candidate, move_x, move_y);

            if (t.type == prey) {
                if (candidate.type == grass) {
                    t.food += candidate.food;
                    if (t.food > prey_reproduce_above) {
                        t.food -= prey_reproduce_cost;
                        candidate.type = prey;
                        candidate.food = prey_starting_food;
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
                    t.food += prey_food_value;
                    if (t.food > pred_reproduce_above) {
                        t.food -= pred_reproduce_cost;
                        candidate.type = predator;
                        candidate.food = pred_starting_food;
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

void rule_predator_prey_draw(gef_context *gc, grid g) {
    for (int i = 0; i < g.w; i++) {
        for (int j = 0; j < g.h; j++) {
            tile t;
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