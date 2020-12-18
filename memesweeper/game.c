#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "game.h"
#include "grid.h"
#include "util.h"
#include "gef.h"

game game_init(int n_bombs, int w, int h) {
    game g = {0};
    g.grid = grid_init(sizeof(tile), w, h);

    if (n_bombs >= (w*h - 10)) {
        printf("maybe try less bombs\n");
        exit(1);
    }

    for (int i = 0; i < n_bombs; i++) {
        while (true) {
            int x = util_rand_ab(0, w);
            int y = util_rand_ab(0, h);

            tile t;
            grid_get(g.grid, &t, x, y);
            if (!t.is_bomb) {
                t.is_bomb = true;
                t.is_flag = false;
                grid_set(g.grid, &t, x, y);
                break;
            }
        }    
    }

    g.first_dig = true;
    return g;
}

void game_reset(game *g, int n_bombs, int w, int h) {
    grid_delete(g->grid);
    *g = game_init(n_bombs, w, h);
}

SDL_Rect bomb_clip = {16, 16, 16, 16};
SDL_Rect flag_clip = {32, 16, 16, 16};
SDL_Rect unrevealed_clip = {0, 16, 16, 16};
SDL_Rect revealed_clips[] = {
    {0, 0, 16, 16},
    {16, 0, 16, 16},
    {32, 0, 16, 16},
    {48, 0, 16, 16},

    {64, 0, 16, 16},
    {80, 0, 16, 16},
    {96, 0, 16, 16},
    {128, 0, 16, 16},

    {144, 0, 16, 16},
};

void game_draw(game *g) {
    int tile_xres = gef_xres() / g->grid.w;
    int tile_yres = gef_yres() / g->grid.h;

    for (int x = 0; x < g->grid.w; x++) {
        for (int y = 0; y < g->grid.h; y++) {
            SDL_Rect to_rect = {
                x * tile_xres,
                y * tile_yres,
                tile_xres,
                tile_yres,
            };
            tile t;
            if (!grid_get(g->grid, &t, x, y)) {
                printf("failed to get for drawing for some reason\n");
                exit(1);
            }
            if (t.is_revealed) {
                if (t.is_bomb) {
                    gef_draw_sprite(bomb_clip, to_rect);
                } else {
                    gef_draw_sprite(revealed_clips[t.num_bomb_neighbours], to_rect);
                }
            } else {
                if (t.is_flag) {
                    gef_draw_sprite(flag_clip, to_rect);
                } else {
                    gef_draw_sprite(unrevealed_clip, to_rect);
                }
            }
        }
    }
}

game_response game_dig(game *g, int x, int y) {
    tile t = {0};
    grid_get(g->grid, &t, x, y);

    t.is_revealed = true;
    
    if (t.is_bomb) {
        grid_set(g->grid, &t, x, y);
        return RESP_DEAD;
    }
    
    for (int i = 0; i < 8; i++) {
        tile neigh;
        if (grid_get_neighbour8(g->grid, &neigh, x, y, i)) {
            if (neigh.is_bomb) {
                t.num_bomb_neighbours++;
            }
        }
    }
    grid_set(g->grid, &t, x, y);

    if (t.num_bomb_neighbours == 0) {
        for (int i = 0; i < 8; i++) {
            tile neigh;
            if (grid_get_neighbour8(g->grid, &neigh, x, y, i)) {
                if (!neigh.is_revealed && !neigh.is_flag) {
                    int nx;
                    int ny;
                    grid_index_neighbour8(g->grid, x, y, i, &nx, &ny);
                    game_dig(g, nx, ny); // y infinite recursion
                }
                if (neigh.is_bomb) {
                    t.num_bomb_neighbours++;
                }
            }
        }
    }

    return RESP_OK;
}

game_response game_first_dig(game *g, int x, int y) {
    int startx = max(x-1, 0);
    int starty = max(y-1, 0);
    int endx = min(x+1, g->grid.w);
    int endy = min(y+1, g->grid.h);

    // check starting (up to) 3x3 square
    for (int i = startx; i <= endx; i++) {
        for (int j = starty; j <= endy; j++) {

            tile t = {0};
            grid_get(g->grid, &t, i, j);
            
            // if it needs replacing
            if (t.is_bomb) {
                t.is_bomb = false;
                grid_set(g->grid, &t, i, j);

                while (true) {
                    int x = util_rand_ab(0, g->grid.w);
                    int y = util_rand_ab(0, g->grid.h);

                    // select random numbers until its not in the square
                    if (x >= startx && x <= endx && y >= starty && y <= endy) {
                        continue;
                    }

                    // and not a bomb
                    tile t;
                    grid_get(g->grid, &t, x, y);
                    if (t.is_bomb) {
                        t.is_bomb = true;
                        grid_set(g->grid, &t, x, y);
                        break;
                    }
                }
            }
        }
    }
    
    return game_dig(g, x, y);
}

bool check_victory(game *g) {
    for (int i = 0; i < g->grid.w; i++) {
        for (int j = 0; j < g->grid.h; j++) {
            tile t;
            grid_get(g->grid, &t, i, j);
            if (!(t.is_revealed || t.is_bomb)) {
                return false;
            }
        }
    }
    return true;
}

game_response handle_tile_click(game *g, int x, int y, bool lmb) {
    tile t;
    if (!grid_get(g->grid, &t, x, y)) {
        printf("bad tile click");
        return RESP_OK;
    }

    if (t.is_revealed) {
        return RESP_OK;
    }

    if (lmb) {
        if (t.is_flag) {
            return RESP_OK;
        }
        if (t.is_bomb) {
            t.is_revealed = true;
            grid_set(g->grid, &t, x, y);
            return RESP_DEAD;
        } else if (g->first_dig) {
            g->first_dig = false;
            game_first_dig(g, x, y);
            if (check_victory(g)) {
                return RESP_VICTORY;
            } else {
                return RESP_OK;
            }
        } else {
            game_dig(g, x, y);
            if (check_victory(g)) {
                return RESP_VICTORY;
            } else {
                return RESP_OK;
            }
        }
    } else {
        // rmb -- toggle flag
        if (t.is_flag) {
            t.is_flag = false;
        } else {
            t.is_flag = true;
        }
        grid_set(g->grid, &t, x, y);
        return RESP_OK;
    }
}
