#include <string.h>

#include "gef.h"
#include "world.h"
#include "util.h"

/*
world world_init(int xres, int yres) {
    const int starting_array_size = 10;

    entity *entities = calloc(starting_array_size, sizeof(entity));

    grid terrain = grid_init(sizeof(tile), xres, yres);

    // set up the world
    for (int i = 0; i < xres; i++) {
        for (int j = 0; j < yres; j++) {
            tile t = (tile) {
                .fert = 0.0001,
                .growth = 0,
            };
            grid_set(terrain, &t, i, j);
        }
    }
    return (world) {
        .backing_array_size = starting_array_size,
        .entities = entities,
        .num_entities = 0,
        .terrain = terrain,
    };

}

void world_add_entity(world *w, entity e) {
    // first try and overwrite
    for (int i = 0; i < w->num_entities; i++) {
        if (w->entities[i].alive == false) {
            w->entities[i] = e;
            return;
        }
    }

    // then check if we need to realloc
    if (w->num_entities == w->backing_array_size) {
        w->entities = realloc(w->entities, sizeof(entity) * w->backing_array_size*2);
        memset(w->entities + w->backing_array_size, 0, sizeof(entity)*w->backing_array_size);
        w->backing_array_size *= 2;
    }
    w->entities[w->num_entities++] = e;
}

// todo die from overcrowding if their food gets drained
void world_rule_city(world *w, int ent_idx) {
    entity *e = &w->entities[ent_idx];

    if (e->et == ENT_CITY) {
        return;
    }

    // move to a random existing neighbouring square
    while (!grid_index_neighbour8(w->terrain, e->x, e->y, util_rand_ab(0, 8), &e->x, &e->y));

    tile t;
    grid_get(w->terrain, &t, e->x, e->y);

    // harvest food
    const float max_food_carry = 1;
    float remaining_space = max_food_carry - e->carried_food;
    float amount_of_food_harvested = min(remaining_space, t.growth);

    t.growth -= amount_of_food_harvested;
    e->carried_food += amount_of_food_harvested;

    // check for interactions
    for (int i = 0; i < w->num_entities; i++) {
        entity *other_e = &w->entities[i];
        if (!other_e->alive) continue;

        bool same_square = other_e->x == e->x && other_e->y == e->y;
        if (!same_square) continue;



        if (other_e->team != e->team) {
            // enemies
            if (other_e->et == ENT_DUDE) {
                bool we_win = util_rand_ab(0,1);
                if (we_win) {
                    other_e->alive = false;
                } else {
                    e->alive = false;
                }
            } else if (other_e->et == ENT_CITY) {
                other_e->health -= 0.2;
                if (other_e->health <= 0) {
                    other_e->alive = false;
                }
            }
        } else {
            // friends might make a city
            if (other_e->et == ENT_DUDE) {
                //return; // just disabling for now
                if (e->carried_food == 1 && other_e->carried_food == 1) {
                    e->carried_food = 0;
                    other_e->carried_food = 0;

                    int new_team = util_rand_ab(0, 10000000);
                    e->team = new_team;
                    other_e->team = new_team;
                    world_add_entity(w, (entity) {
                        .alive = true,
                        .carried_food = 0,
                        .health = 1,
                        .team = new_team,
                        .x = e->x,
                        .y = e->y,
                        .et = ENT_CITY,
                    });
                    world_add_entity(w, (entity) {
                        .alive = true,
                        .carried_food = 0,
                        .et = ENT_DUDE,
                        .team = e->team,
                        .x = e->x,
                        .y = e->y,
                    });
                    world_add_entity(w, (entity) {
                        .alive = true,
                        .carried_food = 0,
                        .et = ENT_DUDE,
                        .team = e->team,
                        .x = e->x,
                        .y = e->y,
                    });
                }
            } else if (other_e->et == ENT_CITY) {
                other_e->carried_food += e->carried_food;
                e->carried_food = 0;
                if (other_e->carried_food > 1) {
                    other_e->carried_food -= 1;
                    world_add_entity(w, (entity) {
                        .alive = true,
                        .carried_food = 0,
                        .et = ENT_DUDE,
                        .team = e->team,
                        .x = e->x,
                        .y = e->y,
                    });
                }

            }
        }
    }
}
void world_rule_predator_prey(world *w, int ent_idx) {
    const int predator = 0;
    const int prey = 1;



    entity *e = &w->entities[ent_idx];

    // move to a random existing neighbouring square
    while (!grid_index_neighbour8(w->terrain, e->x, e->y, util_rand_ab(0, 8), &e->x, &e->y));

    tile t;
    grid_get(w->terrain, &t, e->x, e->y);

    if (e->et == predator) {
        e->carried_food -= pred_hunger_rate;
        if (e->carried_food <= 0) {
            e->alive = false;
            return;
        }
    } else if (e->et == prey) {
        e->carried_food += t.growth;
        if (e->carried_food > prey_reproduce_above) {
            e->carried_food -= prey_reproduce_cost;
            world_add_entity(w, (entity) {
                .alive = true,
                .carried_food = prey_starting_food,
                .et = prey,
                .team = prey,
                .x = e->x,
                .y = e->y,
            });
        }
    }


    for (int i = 0; i < w->num_entities; i++) {
        entity *other_e = &w->entities[i];
        if (!other_e->alive) continue;

        bool same_square = other_e->x == e->x && other_e->y == e->y;
        if (!same_square) continue;

        if (e->et == predator) {
            if (other_e->et == prey) {
                other_e->alive = false;
                e->carried_food += prey_food_value;
                if (e->carried_food > pred_reproduce_above) {
                    e->carried_food -= pred_reproduce_cost;
                    world_add_entity(w, (entity) {
                        .alive = true,
                        .carried_food = prey_starting_food,
                        .et = predator,
                        .team = predator,
                        .x = e->x,
                        .y = e->y,
                    });
                }
            }
        }
    }

}
*/
/*
void world_step(world *w, void (*rule)(world*, int)) {
    // update terrain
    for (int i = 0; i < w->terrain.w; i++) {
        for (int j = 0; j < w->terrain.h; j++) {
            tile t;
            grid_get(w->terrain, &t, i, j);

            t.growth += t.fert;
            if (t.growth > 1) {
                t.growth = 1;
            }

            grid_set(w->terrain, &t, i, j);
        }
    }

    // update ents
    for (int i = 0; i < w->num_entities; i++) {
        if (w->entities[i].alive) {
            rule(w, i);
        }
    }
}

void world_draw(world *w, gef_context *gc) {
    for (int i = 0; i < w->terrain.w; i++) {
        for (int j = 0; j < w->terrain.h; j++) {
            tile t;
            grid_get(w->terrain, &t, i, j);

            int g = t.growth * 255;

            gef_put_pixel(gc, 0, g, 0, 255, i, j);
        }
    }

    for (int i = 0; i < w->num_entities; i++) {
        if (w->entities[i].alive) {
            int ent_r = hash_intn(w->entities[i].team + 4345324, 0, 255);
            int ent_g = hash_intn(w->entities[i].team + 8973242, 0, 255);
            int ent_b = hash_intn(w->entities[i].team + 4353457, 0, 255);

            gef_put_pixel(gc, ent_r, ent_g, ent_b, 255, w->entities[i].x, w->entities[i].y);
        }
    }


}
*/