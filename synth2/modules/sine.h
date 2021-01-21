#pragma once

#include "module.h"

void sine_get_audio(module *m, module_manager *mm, float *buf);
module module_sine_create(double w);