#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "grid.h"

grid grid_init(size_t size, int w, int h) {
    grid g = {0};
    g.size = size;
    g.w = w;
    g.h = h;
    g.data = calloc(w*h, size);
    return g;
}

// buf: size
void grid_set(grid g, void *buf, int x, int y) {
    int index = y * g.w + x;
    memcpy(g.data + (index * g.size), buf, g.size);
}

// buf: size
bool grid_get(grid g, void *buf, int x, int y) {
    if (x < 0 || x > g.w || y < 0 || y > g.h) {
        return false;
    }
    int index = y * g.w + x;
    memcpy(buf, g.data + (index * g.size), g.size);
    return true;
}

int x_neigh_indices[] = {-1,  0,  1, -1,  1, -1,  0, 1};
int y_neigh_indices[] = {-1, -1, -1,  0,  0,  1,  1, 1};

bool grid_validate_indices(grid g, int x, int y) {
    return (x >= 0 && x < g.w && y >= 0 && y < g.h);
}

// buf: size
// return is neighbour
bool grid_get_neighbour8(grid g, void *buf, int x, int y, int n) {

    if (n > 9) {
        printf("bad neighbour index grid_get_neighbour8\n");
        exit(1);
    }
    int newx = x + x_neigh_indices[n];
    int newy = y + y_neigh_indices[n];

    if (!grid_validate_indices(g, newx, newy)) return false;

    grid_get(g, buf, newx, newy);
    return true;
}

bool grid_index_neighbour8(grid g, int x, int y, int n, int *xp, int *yp) {

    if (n > 9) {
        printf("bad neighbour index grid_index_neighbour8\n");
        exit(1);
    }
    int newx = x + x_neigh_indices[n];
    int newy = y + y_neigh_indices[n];

    if (!grid_validate_indices(g, newx, newy)) return false;

    *xp = newx;
    *yp = newy;

    return true;
}

void grid_delete(grid g) {
    free(g.data);
}