#pragma once

#include "mesh.h"
#include "pnc_vertex.h"

// -1 to 1 then scale it as u wish
void generate_island(mesh<pnc_vertex> *m, float step, uint32_t seed);