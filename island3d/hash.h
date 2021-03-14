#pragma once
#include <stdint.h>
#include <glm/glm.hpp>

uint32_t hash(int position);
float hash_floatn(int position, float min, float max);
float hash_intn(int position, int min, int max);
float hash_noise2(glm::vec2 v, int seed);
float hash_fbm2_4(glm::vec2 v, int seed);

float hash_circ_noise(float theta, uint32_t seed, int bins);
float hash_circ_fbm3(float theta, uint32_t seed, int bins);
float hash_circ_fbm4(float theta, uint32_t seed, int bins);
