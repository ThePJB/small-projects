#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    uint8_t *data;
    size_t size;
    size_t w;
    size_t h;
} grid;

grid grid_init(size_t size, int w, int h);

// buf: size
void grid_set(grid g, void *buf, int x, int y);

// buf: size
bool grid_get(grid g, void *buf, int x, int y);

// buf: size
// return is neighbour
bool grid_get_neighbour8(grid g, void *buf, int x, int y, int n);

bool grid_index_neighbour8(grid g, int x, int y, int i, int *xp, int *yp);

void grid_delete(grid g);