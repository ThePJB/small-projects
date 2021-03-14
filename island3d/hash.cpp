#include "hash.h"
#include "dankmath.h"
#include <glm/glm.hpp>
using glm::vec2;

uint32_t hash(int position) {
    const unsigned int BIT_NOISE1 = 0xB5297A4D;
    const unsigned int BIT_NOISE2 = 0x68E31DA4;
    const unsigned int BIT_NOISE3 = 0x1B56C4E9;

    unsigned int mangled = position;
    mangled *= BIT_NOISE1;
    mangled ^= (mangled >> 8);
    mangled += BIT_NOISE2;
    mangled ^= (mangled << 8);
    mangled *= BIT_NOISE3;
    mangled ^= (mangled >> 8);
    return mangled;
}

#define U32_MAX 0xFFFFFFFF

float hash_floatn(int position, float min, float max) {
    return ((double)hash(position)/U32_MAX) * (max - min) + min;
}

float hash_intn(int position, int min, int max) {
    return (hash(position)%(max - min)) + min;
}

float hash_noise2(vec2 v, int seed) {
    int x1 = dm_floor(v.x);
    int x2 = x1 + 1;
    int y1 = dm_floor(v.y);
    int y2 = y1 + 1;

    return dm_bilinear3(
        hash_floatn(x1 + y1 * 987423234 + seed * 342447, 0, 1), 
        hash_floatn(x2 + y1 * 987423234 + seed * 342447, 0, 1), 
        hash_floatn(x1 + y2 * 987423234 + seed * 342447, 0, 1), 
        hash_floatn(x2 + y2 * 987423234 + seed * 342447, 0, 1), 
        dm_frac(v.x), dm_frac(v.y)
    );
}

// _4 is how many hash_noise2 calls
float hash_fbm2_4(vec2 v, int seed) {
    return (
        1.000 * hash_noise2(1.0f * v, seed + 234243) +
        0.500 * hash_noise2(2.0f * v, seed + 980732) +
        0.250 * hash_noise2(4.0f * v, seed + 895642) +
        0.125 * hash_noise2(8.0f * v, seed + 987443)
    ) / 1.875;
}

float hash_circ_noise(float theta, uint32_t seed, int bins) {

    float start_bin = theta / (2*M_PI) * bins;
    int start_bin_i = (int)start_bin;

    float f1 = hash_floatn(seed + start_bin_i * 2347234, 0, 1);
    float f2 = hash_floatn(seed + ((start_bin_i + 1) % bins)* 2347234, 0, 1);

    float t = dm_frac(start_bin);
    return dm_lerp(f1, f2, t*t*(3-2*t));
}

float hash_circ_fbm3(float theta, uint32_t seed, int bins) {
    return (1.0000 * hash_circ_noise(theta, seed, bins) +
            0.5000 * hash_circ_noise(theta, seed + 435984, 2*bins) +
            0.2500 * hash_circ_noise(theta, seed + 345345, 4*bins)) / 1.75;
}

float hash_circ_fbm4(float theta, uint32_t seed, int bins) {
    return (1.0000 * hash_circ_noise(theta, seed, bins) +
            0.5000 * hash_circ_noise(theta, seed + 435984, 2*bins) +
            0.2500 * hash_circ_noise(theta, seed + 345345, 4*bins) +
            0.1250 * hash_circ_noise(theta, seed + 145345, 8*bins)) / 1.87;
}