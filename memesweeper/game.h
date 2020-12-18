#pragma once

#include <stdbool.h>
#include "grid.h"

typedef struct {
    grid grid;
    bool first_dig;
} game;

typedef struct {
    bool is_bomb;
    bool is_flag;
    bool is_revealed;
    int num_bomb_neighbours;
} tile;

typedef enum {
    RESP_OK,
    RESP_DEAD,
    RESP_VICTORY,
} game_response;

game game_init(int n_bombs, int w, int h);
void game_reset(game *g, int n_bombs, int w, int h);
void game_draw(game *g);

game_response handle_tile_click(game *g, int x, int y, bool lmb);

game_response game_dig(game *g, int x, int y);
game_response game_first_dig(game *g, int x, int y);