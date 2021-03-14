#include "generation.h"
#include "hash.h"
#include "dankmath.h"
#include <math.h>
#include <glm/gtx/normal.hpp>

using glm::vec2;

pnc_vertex island_vertex(float x, float y, uint32_t seed) {
    float r = sqrtf(x*x + y*y);
    float theta = atan2f(y, x) + M_PI;

    float sea_r = hash_circ_fbm4(theta, seed * 23489, 6);


    // yer this h thing is a bit shit, i mean we could have it but meh tbh
    // would need to be more interesting and vary
    // modifying t helps heaps actually
    float tsmooth_factor = hash_circ_fbm4(theta, seed * 3242357, 6);
    float t = fmax((sea_r - r)/sea_r, 0);
    //float tprime = t*t*(3-2*t);
    float tprime = dm_slow_start2(t);
    float h = dm_lerp(0, 0.1, dm_lerp(t, tprime, tsmooth_factor));

    vec3 c = vec3(0, 1, 0);

    if (h < 0.01) c = vec3(1, 1, 0);
    if (h < 0.001) c = vec3(0, 0, 1);

    if (r < sea_r) {
        return pnc_vertex(
            vec3(x, h, y),
            vec3(0, 1, 0),
            c
        );
    } else {
        return pnc_vertex(
            vec3(x, 0, y),
            vec3(0, 0, 1),
            c
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

            vec3 n1 = glm::triangleNormal(v1.pos, v2.pos, v3.pos);
            vec3 n2 = glm::triangleNormal(v2.pos, v4.pos, v3.pos);

            v1.normal = n1;
            v2.normal = n1;
            v3.normal = n1;

            m->push(v1);
            m->push(v2);
            m->push(v3);

            v2.normal = n2;
            v4.normal = n2;
            v3.normal = n2;
            
            m->push(v2);
            m->push(v4);
            m->push(v3);
        }
    }
}