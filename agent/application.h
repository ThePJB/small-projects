#pragma once

#include <stdbool.h>

#include "gef.h"
#include "world.h"

typedef struct {
    gef_context gc;
    bool keep_going;

    bool paused;

    world w;
    void (*rule)(world*, int);

} application;

void application_handle_input(application *app);
void application_draw(application *app);
void application_step(application *app);

application application_init();