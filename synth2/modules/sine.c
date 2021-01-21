#include <math.h>
#include "sine.h"

void sine_get_audio(module *m, module_manager *mm, float *buf) {
    const double sample_duration = 1.0 / mm->sample_rate;

    for (int i = 0; i < CHUNK_SIZE; i++) {
        const double t = m->sine.n++ * sample_duration;
        double w = m->sine.w;
        buf[i] = sin(w*t);
    }
}

module module_sine_create(double w) {
    return (module) {
        .get_audio = sine_get_audio,
        .mt = MT_SINE,
        .sine = {
            .n = 0,
            .w = w,
        }
    };
}