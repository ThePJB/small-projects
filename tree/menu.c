#include "menu.h"
#include "text.h"
#include "mymath.h"

#include <stdlib.h>
#include <stdio.h>

const size_t starting_menu_size = 10;

menu menu_new() {
    return (menu) {
        .items = calloc(starting_menu_size, sizeof(menu_record)),
        .backing_size = starting_menu_size,
        .current_selection = 0,
        .num_items = 0,
    };
}

void menu_push(menu *m, char *label, float *value, float min, float max, float inc) {
    menu_record new_record = (menu_record) {
        .label = label,
        .value = value,
        .min = min,
        .max = max,
        .inc = inc,
    };

    if (m->num_items == m->backing_size) {
        m->backing_size *= 2;
        m->items = realloc(m->items, m->backing_size * sizeof(menu_record));
    }
    m->items[m->num_items++] = new_record;
}

void menu_draw(menu *m, float x, float y) {
    const text_style menu_style = (text_style) {
        .colour = (vec3s) {1, 1, 1},
        .scale = 0.5,
    };
    const float ygap = 30;

    if (m->current_selection != 0) {
        // draw above hint
        char buf[128] = {0};
        sprintf(buf, "%s: %.2f", m->items[m->current_selection-1].label, *m->items[m->current_selection-1].value);
        draw_text(buf, x, y, menu_style);
    }
    // draw selection hint
    char buf[128] = {0};
    sprintf(buf, "> %s: %.2f", m->items[m->current_selection].label, *m->items[m->current_selection].value);
    draw_text(buf, x, y-ygap, menu_style);

    // draw below hint
    if (m->current_selection < m->num_items - 1) {
        char buf[128] = {0};
        sprintf(buf, "%s: %.2f", m->items[m->current_selection+1].label, *m->items[m->current_selection+1].value);
        draw_text(buf, x, y-ygap-ygap, menu_style);
    }
}

void menu_update(menu *m, menu_direction dir) {
    if (dir == MD_UP && m->current_selection > 0) {
        m->current_selection--;
    } else if (dir == MD_DOWN && m->current_selection < m->num_items - 1) {
        m->current_selection++;
    } else if (dir == MD_LEFT) {
        *m->items[m->current_selection].value -= m->items[m->current_selection].inc;
        *m->items[m->current_selection].value = max(*m->items[m->current_selection].value, m->items[m->current_selection].min);
    } else if (dir == MD_RIGHT) {
        *m->items[m->current_selection].value += m->items[m->current_selection].inc;
        *m->items[m->current_selection].value = min(*m->items[m->current_selection].value, m->items[m->current_selection].max);       
    }
}
