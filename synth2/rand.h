#pragma once
#include <stdint.h>

typedef struct {
    uint32_t seed;
} rng;

uint32_t hash(int position);
float hash_floatn(int position, float min, float max);
float hash_intn(int position, int min, int max);
float rand_floatn(rng *rng, float min, float max);
