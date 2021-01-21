#pragma once

#include <stdint.h>

#include "portaudio.h"

typedef struct {
    PaStream *stream;
    PaError err; 
} aud_context;

int aud_test_callback(
        const void *input_buffer_vp, 
        void *output_buffer_vp,
        unsigned long frame_count,
        const PaStreamCallbackTimeInfo* time_info,
        PaStreamCallbackFlags status_flags,
        void *rt_data_vp);

int aud_test_whitenoise_callback(
        const void *input_buffer_vp, 
        void *output_buffer_vp,
        unsigned long frame_count,
        const PaStreamCallbackTimeInfo* time_info,
        PaStreamCallbackFlags status_flags,
        void *rt_data_vp);

//aud_context aud_init(void *rt_data, PaStreamCallback rt_callback, uint32_t sample_rate, uint32_t chunk_size, int device);
aud_context aud_init(void *rt_data, PaStreamCallback rt_callback, double sample_rate, int chunk_size);