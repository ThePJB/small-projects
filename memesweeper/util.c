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