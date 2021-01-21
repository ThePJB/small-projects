#include <stdio.h>
#include <math.h>

#include "aud.h"
#include "rand.h"


void print_pa_err(PaError err) {
    fprintf(stderr, "aud killed by error (%d) %s\n", err, Pa_GetErrorText(err));
}

void aud_teardown(aud_context *ac) {
    ac->err = Pa_StopStream(ac->stream); if (ac->err != paNoError) print_pa_err(ac->err);
    ac->err = Pa_CloseStream(ac->stream); if (ac->err != paNoError) print_pa_err(ac->err);
    Pa_Terminate();
}

rng grng = {0};

int aud_test_whitenoise_callback(
        const void *input_buffer_vp, 
        void *output_buffer_vp,
        unsigned long frame_count,
        const PaStreamCallbackTimeInfo* time_info,
        PaStreamCallbackFlags status_flags,
        void *rt_data_vp) {
    
    float *output_buffer = (float*)output_buffer_vp;

    for (int i = 0; i < frame_count; i++) {
        // white noise
        *output_buffer++ = rand_floatn(&grng, 0, 1);
    }

    return 0;
}

int aud_test_callback(
        const void *input_buffer_vp, 
        void *output_buffer_vp,
        unsigned long frame_count,
        const PaStreamCallbackTimeInfo* time_info,
        PaStreamCallbackFlags status_flags,
        void *rt_data_vp) {
    
    float *output_buffer = (float*)output_buffer_vp;

    const double sample_duration = 1.0 / 48000;
    const double w = 2 * M_PI * 440;

    static int n = 0;

    for (int i = 0; i < frame_count; i++) {
        const double t = n++ * sample_duration;
        *output_buffer++ = sin(w*t);
    }

    return 0;
}

aud_context aud_init(void *rt_data, PaStreamCallback rt_callback, double sample_rate, int chunk_size) {
    printf("initializing audio...\n");
    aud_context a = {0};

    a.err = Pa_Initialize();
    if (a.err != paNoError) {
        print_pa_err(a.err);
        aud_teardown(&a);
    }

    PaStreamParameters output_params;
    output_params.device = Pa_GetDefaultOutputDevice();
    if (output_params.device == paNoDevice) {
        printf("error: no default output device\n");
        aud_teardown(&a);
    }

    output_params.channelCount = 1;
    output_params.sampleFormat = paFloat32;
    output_params.suggestedLatency = Pa_GetDeviceInfo(output_params.device)->defaultLowOutputLatency;
    output_params.hostApiSpecificStreamInfo = NULL;

    a.err = Pa_OpenStream(
        &a.stream,
        NULL, // no input
        &output_params,
        sample_rate,
        chunk_size,
        paClipOff,
        rt_callback,
        rt_data
    );

    if (a.err != paNoError) {
        print_pa_err(a.err);
        aud_teardown(&a);
    }

    a.err = Pa_StartStream(a.stream);
    if (a.err != paNoError) {
        print_pa_err(a.err);
        aud_teardown(&a);
    }

    printf("done\n");
    return a;
}
/*
aud_context aud_init(void *rt_data, PaStreamCallback rt_callback, uint32_t sample_rate, uint32_t chunk_size, int device) {
    const int num_input_channels = 0;
    const int num_output_channels = 1;
    const PaSampleFormat sample_format = paFloat32;
    const unsigned long frames_per_buffer = chunk_size;

    printf("initializing audio...\n");

   aud_context a = {0};

    a.err = Pa_Initialize();
    if (a.err != paNoError) {
        print_pa_err(a.err);
        aud_teardown(&a);
    }
    */
    /*
    int num_devices = Pa_GetDeviceCount();
    printf("%d devices detected:\n", num_devices);
    if(num_devices < 0) {
        a.err = num_devices;
        print_pa_err(a.err);
        aud_teardown(&a);
    }

    const PaDeviceInfo *device_info;
    for(int i = 0; i < num_devices; i++) {
        device_info = Pa_GetDeviceInfo(i);
        printf("\t%d: %s\n", i, device_info->name);
    }

    const int default_device_idx = Pa_GetDefaultOutputDevice();
    printf("default output device %d\n", default_device_idx);
    */
/*
    PaStreamParameters output_params = {0};
    output_params.channelCount = 1;
    output_params.device = device;
    output_params.sampleFormat = sample_format;

    PaStreamParameters input_params = {0};
    input_params.channelCount = 1;
    input_params.device = paNoDevice;
    input_params.sampleFormat = sample_format;
*/
/*
    a.err = Pa_OpenDefaultStream(
            &a.stream,
            num_input_channels,
            num_output_channels,
            sample_format,
            sample_rate,
            frames_per_buffer, // frames per buffer ??
            rt_callback,
            rt_data); 
*/
/*
    a.err = Pa_OpenStream(
        &a.stream,
        &input_params,
        &output_params,
        sample_rate,
        frames_per_buffer,
        0,
        rt_callback,
        rt_data);
*/  
/*
    if (a.err != paNoError) {
        print_pa_err(a.err);
        aud_teardown(&a);
    } 

    a.err = Pa_StartStream(a.stream); if (a.err != paNoError) print_pa_err(a.err);

    if (a.err != paNoError) {
        print_pa_err(a.err);
        aud_teardown(&a);
    } 

    printf("initializing audio complete\n");

    return a;
}
*/