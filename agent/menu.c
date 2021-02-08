#include "menu.h"
#include "util.h"

void menu_draw(gef_context *gc, menu *m, int x, int y) {
    const int spacing = 30;
    const int sel_indent = 30;

    for (int i = 0; i < m->num_items; i++) {
        if (i == m->current_selection) {
            gef_draw_text(gc, m->entries[i], x+sel_indent, y);
        } else {
            gef_draw_text(gc, m->entries[i], x, y);
        }
        y += spacing;
    }
}

void menu_scroll(menu *m, bool up) {
    if (up) {
        m->current_selection = max(m->current_selection - 1, 0);
    } else {
        m->current_selection = min(m->current_selection + 1, m->num_items-1);
    }
}

void menu_edit(menu *m, gef_context *gc, bool next, 
        void *state, 
        void (*menu_succ)(void *state, int index, bool up), 
        bool (*menu_str)(void *state, int index, char *buf)) {

    menu_succ(state, m->current_selection, next);
    gef_destroy_text(m->entries[m->current_selection]);
    char buf[128] = {0};
    menu_str(state, m->current_selection, buf);
    m->entries[m->current_selection] = gef_make_text(gc, m->font, buf, 255, 255, 255);
}

void menu_clear(menu *m) {
    for (int i = 0; i < m->num_items; i++) {
        gef_destroy_text(m->entries[i]);
    }
    m->num_items = 0;
}

void menu_populate(menu *m, gef_context *gc, void *state, 
        bool (*menu_str)(void *state, int index, char *buf)) {
        
    char buf[128] = {0};
    int i = 0;
    while (menu_str(state, i++, buf)) {
        m->entries[i-1] = gef_make_text(gc, m->font, buf, 255, 255, 255);
        m->num_items++;
    }
}