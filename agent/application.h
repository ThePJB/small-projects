#pragma once

#include <stdbool.h>

#include "gef.h"
#include "world.h"

typedef struct {
    gef_context gc;
    bool keep_going;

    bool paused;

    grid g;
    void (*init_rule)(grid);
    void (*update_rule)(grid);
    void (*draw_rule)(gef_context *, grid);

} application;

void application_handle_input(application *app);
void application_draw(application *app);
void application_step(application *app);

application application_init();