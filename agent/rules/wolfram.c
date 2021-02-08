#include "wolfram.h"
#include "util.h"

// nb 30 chaos
// nb 110 turing complete
// nb 90 sierpinski triangle, its also awesome with a messy input
// nb 102 right sierpinski triangle
// 105 forest with buildings
// around 99 gets some cool mc escher if u go between
// 150 is 2x 102

// 108 109 and back is cool it makes faces

const float starting_on_chance = 0.5;

void rule_wolfram_reset_grid(void *state, int xres, int yres) {
    wolfram_context *wc = (wolfram_context*)state;
    for (int i = 0; i < wc->g[0].w; i++) {
        for (int j = 0; j < wc->g[0].h; j++) {

            bool on = (i ==wc->g[0].w/2 && j == 0);
            
            grid_set(wc->g[0], &on, i, j);
        }
    }
}

void *rule_wolfram_init(int xres, int yres) {
    wolfram_context *state = calloc(1, sizeof(wolfram_context));
    state->g[0] = grid_init(sizeof(bool), xres, yres);
    state->g[1] = grid_init(sizeof(bool), xres, yres);
    rule_wolfram_reset_grid(state, xres, yres);
    state->rule = 30;
    return state;
}

void rule_wolfram_reset_grid_random(void *state, int xres, int yres) {
    wolfram_context *wc = (wolfram_context*)state;
    for (int i = 0; i < wc->g[0].w; i++) {
        for (int j = 0; j < wc->g[0].h; j++) {

            bool on = rand_floatn(0, 1) > starting_on_chance && j == 0;
            
            grid_set(wc->g[0], &on, i, j);
        }
    }
}

void *rule_wolfram_init_random(int xres, int yres) {
    wolfram_context *state = calloc(1, sizeof(wolfram_context));
    state->g[0] = grid_init(sizeof(bool), xres, yres);
    state->g[1] = grid_init(sizeof(bool), xres, yres);
    rule_wolfram_reset_grid_random(state, xres, yres);
    state->rule = 73;
    return state;
}

void rule_wolfram_update(void *state) {    
    wolfram_context *wc = (wolfram_context*)state;

    grid this_g = wc->g[wc->curr_buffer];
    grid next_g = wc->g[1 - wc->curr_buffer];

    int non = 0;
    int nempty = 0;

    for (int i = 0; i < this_g.w; i++) {
        for (int j = 0; j < this_g.h; j++) {
            if (j == 0) {
                bool neigh_left;
                bool this;
                bool neigh_right;

                grid_get(this_g, &neigh_left, (i+this_g.w-1)%this_g.w, j);
                grid_get(this_g, &this, i, j);
                grid_get(this_g, &neigh_right, (i+1)%this_g.w, j);

                if (this) {
                    non++;
                } else {
                    nempty++;
                }

                uint8_t lookup_number = neigh_left << 2 | this << 1 | neigh_right;
                bool value = (wc->rule >> lookup_number) & 1;
                grid_set(next_g, &value, i, j);
            } else {
                // just copy from above to make history
                bool above;
                grid_get(this_g, &above, i, j-1);
                grid_set(next_g, &above, i, j);
            }
        }
    }

    wc->curr_buffer = 1 - wc->curr_buffer;
    printf("generation %d: %d alive, %d dead\n", wc->generation++, non, nempty);
}

void rule_wolfram_draw(gef_context *gc, void *state) {
    wolfram_context *wc = (wolfram_context*)state;
    grid g = wc->g[wc->curr_buffer];
    for (int i = 0; i < g.w; i++) {
        for (int j = 0; j < g.h; j++) {
            bool t;
            grid_get(g, &t, i, j);

            if (t) {
                gef_put_pixel(gc, 255, 255, 255, 255, i, j);
            } else {
                gef_put_pixel(gc, 0, 0, 0, 255, i, j);
            }
        }
    }
}

bool rule_wolfram_menu(void *state, int index, char *buf) {
    wolfram_context *wc = (wolfram_context*)state;

    if (index == 0) {
        sprintf(buf, "rule %d", wc->rule);
        return true;
    } else {
        return false;
    }
}

void rule_wolfram_succ(void *state, int index, bool up) {
    wolfram_context *wc = (wolfram_context*)state;

    if (index == 0) {
        if (up) {
            wc->rule++;
        } else {
            wc->rule--;
        }
        
    }
}