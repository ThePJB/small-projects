#pragma once

#include <stdint.h>

char *slurp(const char *path);
#define len(X) (sizeof(X) / sizeof(X[0]))
uint64_t get_us(); 
float rand_floatn(float min, float max);