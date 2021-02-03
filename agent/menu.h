#pragma once

#include "gef.h"
#include "stdbool.h"

#define MAX_MENU_ITEMS 64

typedef struct {
    int num_items;
    int current_selection;
    text_handle entries[MAX_MENU_ITEMS];
    font_handle font;
} menu;

void menu_draw(gef_context *gc, menu *m, int x, int y);
void menu_scroll(menu *m, bool up); // just changing selection and bounds clamping it
void menu_edit(menu *m, gef_context *gc, bool next, 
        void *state, 
        void (*menu_succ)(void *state, int index, bool up), 
        bool (*menu_str)(void *state, int index, char *buf));

void menu_clear(menu *m);
void menu_populate(menu *m, gef_context *gc, void *state, 
        bool (*menu_str)(void *state, int index, char *buf));
        // and update the text as well
// maybe I will just make what CA be number keys
// then the menu gets deleted and rebuilt
