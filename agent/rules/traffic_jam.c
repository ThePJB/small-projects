#include "traffic_jam.h"
#include "util.h"

// Biham–Middleton–Levine traffic model

const int empty = 0;
const int red = 1;
const int blue = 2;

const float starting_red_chance = 0.2;
const float starting_blue_chance = 0.2;

bool random_creation = false; // random sucks

void *rule_traffic_jam_init(int xres, int yres) {
    traffic_jam_context *state = calloc(1, sizeof(traffic_jam_context));
    state->g[0] = grid_init(sizeof(traffic_jam_tile), xres, yres);
    state->g[1] = grid_init(sizeof(traffic_jam_tile), xres, yres);
    for (int i = 0; i < state->g[0].w; i++) {
        for (int j = 0; j < state->g[0].h; j++) {
            traffic_jam_tile t;
            float r = rand_floatn(0, 1);
            if (r < starting_red_chance) {
                t.type = red;
            } else if (r < starting_red_chance + starting_blue_chance) {
                t.type = blue;
            } else {
                t.type = empty;
            }
            grid_set(state->g[0], &t, i, j);
            //grid_set(state->g[1], &t, i, j);
        }
    }
    return state;
}

void rule_traffic_jam_update(void *state) {
    traffic_jam_context *tjc = (traffic_jam_context*)state;
    grid this_g = tjc->g[tjc->curr_buffer];
    grid next_g = tjc->g[1 - tjc->curr_buffer];

    int nred = 0;
    int nblue = 0;
    int nempty = 0;

    for (int i = 0; i < this_g.w; i++) {
        for (int j = 0; j < this_g.h; j++) {
            bool blue_turn = tjc->curr_buffer == 0;

            traffic_jam_tile this_tile;
            traffic_jam_tile neighbour_tile;
            grid_get(this_g, &this_tile, i, j);

            if (random_creation && i == 0) {
                if (rand_floatn(0, 1) < starting_blue_chance) {
                    this_tile.type = blue;
                } else {
                    this_tile.type = empty;
                }
            } else if (random_creation && j == 0) {
                if (rand_floatn(0, 1) < starting_red_chance) {
                    this_tile.type = red;
                } else {
                    this_tile.type = empty;
                }
            } else if (this_tile.type == empty) {
                nempty++;
                int nx = (i+this_g.w-1)%this_g.w;
                int ny = (j+this_g.h-1)%this_g.h;

                if (blue_turn) {
                    grid_get(this_g, &neighbour_tile, nx, j);
                    if (neighbour_tile.type == blue) {
                        this_tile.type = blue;
                    }
                } else {
                    grid_get(this_g, &neighbour_tile, i, ny);
                    if (neighbour_tile.type == red) {
                        this_tile.type = red;
                    }
                }
            } else if (this_tile.type == blue) {
                nblue++;
                if (blue_turn) {
                    int nx = (i+1)%this_g.w;
                    grid_get(this_g, &neighbour_tile, nx, j);
                    if ((random_creation && nx == 0) || neighbour_tile.type == empty) {
                        this_tile.type = empty;
                    }
                }
            } else if (this_tile.type == red) {
                nred++;
                if (!blue_turn) {
                    int ny = (j+1)%this_g.h;
                    grid_get(this_g, &neighbour_tile, i, ny);
                    if ((random_creation && ny == 0) || neighbour_tile.type == empty) {
                        this_tile.type = empty;
                    }
                }
            }
            grid_set(next_g, &this_tile, i, j);
        }
    }

    tjc->curr_buffer = 1 - tjc->curr_buffer;
    printf("generation %d: %d blue, %d red, %d empty\n", tjc->generation++, nblue, nred, nempty);
}

void rule_traffic_jam_draw(gef_context *gc, void *state) {
    traffic_jam_context *tjc = (traffic_jam_context*)state;
    grid g = tjc->g[tjc->curr_buffer];
    for (int i = 0; i < g.w; i++) {
        for (int j = 0; j < g.h; j++) {
            traffic_jam_tile t;
            grid_get(g, &t, i, j);

            if (t.type == empty) {
                gef_put_pixel(gc, 255, 255, 255, 255, i, j);
            } else if (t.type == blue) {
                gef_put_pixel(gc, 0, 0, 255, 255, i, j);
            } else if (t.type == red) {
                gef_put_pixel(gc, 255, 0, 0, 255, i, j);
            }
        }
    }
}