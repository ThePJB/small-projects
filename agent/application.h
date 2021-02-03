#pragma once

#include <stdbool.h>

#include "menu.h"
#include "gef.h"

typedef struct {
    void *(*init)(int xres, int yres);
    void (*update)(void *);
    void (*draw)(gef_context *, void *);
    void (*reset)(void *state, int xres, int yres);
    bool (*menu_str)(void *state, int index, char *buf);
    void (*menu_succ)(void *state, int index, bool up);
} rule;

typedef struct {
    int xres;
    int yres;
    gef_context gc;
    bool keep_going;
    bool paused;
    void *simulation_state;
    rule current_rule;
    int current_rule_idx;
    menu menu;
    bool show_menu;
} application;

void application_handle_input(application *app);
void application_draw(application *app);
void application_step(application *app);

application application_init();