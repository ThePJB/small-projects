#pragma once
#include <stdint.h>

uint32_t hash(int position);
float hash_floatn(int position, float min, float max);
float hash_intn(int position, int min, int max);
float hash_noise2(float x, float y, int seed);
float hash_fbm2_4(float x, float y, int seed);
