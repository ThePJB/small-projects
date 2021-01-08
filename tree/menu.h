#pragma once

#include <stdlib.h>

// just floats first but ints and enums and bools whatever could come later
// is it a bit gross that it stores ptr to value? we will see
// cause its not labeled in the menu obviously

typedef struct {
    char *label;
    float *value;
    float min;
    float max;
    float inc;
} menu_record;

typedef struct {
    menu_record *items;
    size_t num_items;
    size_t backing_size;

    int current_selection;
} menu;

typedef enum {
    MD_UP,
    MD_DOWN,
    MD_LEFT,
    MD_RIGHT,
} menu_direction;

menu menu_new();
void menu_push(menu *m, char *label, float *value, float min, float max, float inc);
void menu_draw(menu *m, float x, float y);
void menu_update(menu *m, menu_direction dir);
