#include <string.h>

#include <math.h>
#include "module.h"
#include "sine.h"

int module_manager_rt_callback(
        const void *input_buffer_vp, 
        void *output_buffer_vp,
        unsigned long frame_count,
        const PaStreamCallbackTimeInfo* time_info,
        PaStreamCallbackFlags status_flags,
        void *rt_data_vp) {
    
    module_manager *mm = (module_manager*)rt_data_vp;
    float *output_buffer = (float*)output_buffer_vp;

    // extract audio from the sink
    module_manager_get_audio(mm, 0, 0, output_buffer);

    return 0;
}

module_manager module_manager_init(uint32_t sample_rate) {
    module_manager mm = {0};
    mm.sample_rate = sample_rate;
    mm.modules[0].mt = MT_SINK;
    mm.modules[1] = module_sine_create(440 * 2 * M_PI);
    module_manager_connect(&mm, 0, 1, 0);
    return mm;
}

void module_manager_connect(module_manager *mm, int parent, int child, int connection_number) {
    int combined_index = parent << MAX_CHILDREN_BITS | connection_number;

    mm->child_connections[combined_index] = child;
}

// connection number of child
void module_manager_get_audio(module_manager *mm, int parent, int connection_number, float *buf) {
    int combined_index = parent << MAX_CHILDREN_BITS | connection_number;
    module *child_ptr = &mm->modules[mm->child_connections[combined_index]];
    if (child_ptr->mt != MT_NONE) {
        child_ptr->get_audio(child_ptr, mm, buf);
    } else {
        // no connection, have some zeros
        // maybe assume already 0 and return
        return;
    }
}