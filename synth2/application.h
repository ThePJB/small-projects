#pragma once

#include <stdbool.h>

#include "module.h"
#include "gef.h"
#include "aud.h"

typedef struct {
    gef_context gc;
    aud_context ac;
    module_manager mm;
    
    bool keep_going;

} application;

void application_handle_input(application *app);
void application_draw(application *app);
void application_init(application *app);
