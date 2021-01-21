#pragma once

#include <stdint.h>
#include <SDL.h>
#include "portaudio.h"
#include "gef.h"

#define CHUNK_SIZE 100
#define MAX_MODULES 32

// regrettably for tagged union this may have to live here
// or sine isnt allowed to know about this file
// could probably do some disgusting preprocessor stuff to accomplish that,
// like if the modules provide a func to init their inner part, stitch that with 
// their fp etc using a macro to provide a function that creates a module
// will snooze on it for now lol
typedef struct {
    int w;
    int n;
} mod_sine;

typedef enum {
    MT_NONE,
    MT_SINK,
    MT_SINE,
    MT_GATE,
} module_type;

struct module; 
struct module_manager;

typedef struct module {
    module_type mt;
    SDL_Rect position;
    void (*get_audio)(struct module_manager *mm, int m_index, float *data);
    union {
        mod_sine sine;
    };
} module;

// managers modules (array from 0 to MAX_MODULES)
// and connections (index to generation + index and check correctness)
#define MAX_CHILDREN_BITS 4

typedef struct module_manager {
    module modules[MAX_MODULES];

    // so this is assosciative
    int child_connections[MAX_MODULES << MAX_CHILDREN_BITS];

    uint32_t sample_rate;
} module_manager;

module_manager module_manager_init(uint32_t sample_rate);
void module_manager_connect(module_manager *mm, int parent, int child, int index);
void module_manager_get_audio(module_manager *mm, int parent, int connection_number, float *buf);
void module_manager_draw(module_manager *mm, gef_context *gc);

int module_manager_rt_callback(
        const void *input_buffer_vp, 
        void *output_buffer_vp,
        unsigned long frame_count,
        const PaStreamCallbackTimeInfo* time_info,
        PaStreamCallbackFlags status_flags,
        void *rt_data_vp);