#include <stdlib.h>
#include <time.h>
#include "util.h"

int util_rand_ab(int a, int b) {
    return a + rand() % b;
}

uint64_t get_us() { 
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)t.tv_sec * 1000000 + (uint64_t)t.tv_nsec / 1000;
}

float rand_floatn(float min, float max) {
    return rand() / (double)RAND_MAX * (max-min) + min;
}

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