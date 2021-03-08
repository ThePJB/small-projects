#include "hash.h"
#include "coolmath.h"

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

float hash_noise2(float x, float y, int seed) {
    int x1 = cm_floor(x);
    int x2 = x1 + 1;
    int y1 = cm_floor(y);
    int y2 = y1 + 1;

    return cm_bilinear3(
        hash_floatn(x1 + y1 * 987423234 + seed * 342447, 0, 1), 
        hash_floatn(x2 + y1 * 987423234 + seed * 342447, 0, 1), 
        hash_floatn(x1 + y2 * 987423234 + seed * 342447, 0, 1), 
        hash_floatn(x2 + y2 * 987423234 + seed * 342447, 0, 1), 
        cm_frac(x), cm_frac(y)
    );
}

// _4 is how many hash_noise2 calls
float hash_fbm2_4(float x, float y, int seed) {
    return (
        1.000 * hash_noise2(x * 1, y * 1, seed + 234243) +
        0.500 * hash_noise2(x * 2, y * 2, seed + 980732) +
        0.250 * hash_noise2(x * 4, y * 4, seed + 895642) +
        0.125 * hash_noise2(x * 8, y * 8, seed + 987443)
    ) / 1.875;
}