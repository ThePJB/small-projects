#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "module.h"
#include "sine.h"
#include "gate.h"
#include "gef.h"
#include "util.h"

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
    memset(mm.child_connections, -1, sizeof(mm.child_connections));
    mm.sample_rate = sample_rate;
    mm.modules[0].mt = MT_SINK;
    mm.modules[0].position = (SDL_Rect) {200, 100, 50, 50};
    mm.modules[1] = module_sine_create((SDL_Rect){0,0,50,50}, 440 * 2 * M_PI);
    mm.modules[2] = module_sine_create((SDL_Rect){0,60,50,50}, 1 * 2 * M_PI);
    mm.modules[3] = module_gate_create((SDL_Rect){60,60,50,50});
    module_manager_connect(&mm, 3, 1, 0);
    module_manager_connect(&mm, 3, 2, 1);
    module_manager_connect(&mm, 0, 3, 0);
    return mm;
}

void module_manager_connect(module_manager *mm, int parent, int child, int connection_number) {
    int combined_index = parent << MAX_CHILDREN_BITS | connection_number;

    mm->child_connections[combined_index] = child;
}

// connection number of child
void module_manager_get_audio(module_manager *mm, int parent, int connection_number, float *buf) {
    int connection_index = parent << MAX_CHILDREN_BITS | connection_number;
    int child_index = mm->child_connections[connection_index];
    module *child_ptr = &mm->modules[child_index];
    if (child_ptr->mt != MT_NONE) {
        child_ptr->get_audio(mm, child_index, buf);
    } else {
        // no connection, have some zeros
        // maybe assume already 0 and return
        return;
    }
}

void module_manager_draw(module_manager *mm, gef_context *gc, SDL_Point mouse_pos) {
    for (int i = 0; i < MAX_MODULES; i++) {
        if (mm->modules[i].mt != MT_NONE) {
            module *m = &mm->modules[i];
            // rollover module?
            bool module_rollover = SDL_PointInRect(&mouse_pos, &m->position);
            if (module_rollover) {
                gef_draw_rect(gc, rect_dilate(m->position, 2), 255, 255, 255);
            }
            gef_draw_rect(gc, m->position, 50, 50, 50);

            for (int j = 0; j < 1 << MAX_CHILDREN_BITS; j++) {
                int child_index = mm->child_connections[(i << MAX_CHILDREN_BITS) + j];
                if (child_index >= 0) {
                    module *c = &mm->modules[child_index];
                    gef_draw_line(gc, m->position.x, m->position.y, c->position.x, c->position.y, 255, 0, 0);
                    if (dist_point_line_seg(mouse_pos.x, mouse_pos.y, m->position.x, m->position.y, c->position.x, c->position.y) < 2) {
                        gef_draw_line(gc, m->position.x, m->position.y+1, c->position.x, c->position.y+1, 255, 255, 255);
                        gef_draw_line(gc, m->position.x, m->position.y-1, c->position.x, c->position.y-1, 255, 255, 255);
                    }
                }
            }
        }
    }
}