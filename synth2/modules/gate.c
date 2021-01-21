#include <math.h>
#include "gate.h"

// make it varargs - loop over all connections? ask mm if connected?

void gate_get_audio(module_manager *mm, int m_index, float *buf) {
    module_manager_get_audio(mm, m_index, 0, buf);

    // this part loop over remaining
    float buf_child[CHUNK_SIZE] = {0};
    module_manager_get_audio(mm, m_index, 1, buf_child);

    for (int i = 0; i < CHUNK_SIZE; i++) {
        buf[i] *= buf_child[i];
    }
}

module module_gate_create(SDL_Rect position) {
    return (module) {
        .position = position,
        .get_audio = gate_get_audio,
        .mt = MT_GATE,
    };
}