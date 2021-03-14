#include "generation.h"
#include "hash.h"
#include <math.h>

using glm::vec2;

pnc_vertex island_vertex(float x, float y, uint32_t seed) {
    float r = sqrtf(x*x + y*y);
    float theta = atan2f(y, x) + M_PI;

    float sea_r = hash_circ_fbm4(theta, seed * 23489, 7);

    if (r < sea_r) {
        return pnc_vertex(
            vec3(x, 0.1, y),
            vec3(0, 1, 0),
            vec3(0, 1, 0)
        );
    } else {
        return pnc_vertex(
            vec3(x, -0.1, y),
            vec3(0, 0, 1),
            vec3(0, 0, 1)
        );
    }
}

void generate_island(mesh<pnc_vertex> *m, float step, uint32_t seed) {  
    vec2 start = vec2(-1);
    vec2 end = vec2(1);
    
    for (float x = start.x; x < end.x; x += step) {
        for (float y = start.y; y < end.y; y += step) {

            pnc_vertex v1 = island_vertex(x, y, seed);
            pnc_vertex v2 = island_vertex(x+step, y, seed);
            pnc_vertex v3 = island_vertex(x, y+step, seed);
            pnc_vertex v4 = island_vertex(x+step, y+step, seed);

            m->push(v1);
            m->push(v2);
            m->push(v3);
            
            m->push(v2);
            m->push(v4);
            m->push(v3);
        }
    }
}