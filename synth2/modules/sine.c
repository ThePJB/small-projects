#include <math.h>
#include "sine.h"

void sine_get_audio(module_manager *mm, int m_index, float *buf) {
    module *m = &mm->modules[m_index];
    const double sample_duration = 1.0 / mm->sample_rate;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        const double t = m->sine.n++ * sample_duration;
        double w = m->sine.w;
        buf[i] = sin(w*t);
    }
}

module module_sine_create(SDL_Rect position, double w) {
    return (module) {
        .position = position,
        .get_audio = sine_get_audio,
        .mt = MT_SINE,
        .sine = {
            .n = 0,
            .w = w,
        }
    };
}