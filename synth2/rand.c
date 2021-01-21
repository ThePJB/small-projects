#include "rand.h"

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

float rand_floatn(rng *rng, float min, float max) {
    rng->seed = hash(rng->seed);
    return ((double)hash(rng->seed)/U32_MAX) * (max - min) + min;
}