#pragma once

#include "module.h"

void sine_get_audio(module_manager *mm, int m_index, float *buf);
module module_sine_create(SDL_Rect position, double w);