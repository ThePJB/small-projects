#include "application.h"
#include "SDL.h"
#include "predator_prey.h"
#include "traffic_jam.h"
#include "wolfram.h"
#include "menu.h"
#include "util.h"

const rule rules[] = {
    (rule) {
        .draw = rule_predator_prey_draw,
        .init = rule_predator_prey_init,
        .update = rule_predator_prey_update,
        .menu_succ = rule_predator_prey_menu_succ,
        .menu_str = rule_predator_prey_menu_str,
        .reset = rule_predator_prey_reset,
    },
    (rule) {
        .draw = rule_traffic_jam_draw,
        .init = rule_traffic_jam_init,
        .update = rule_traffic_jam_update,
        .menu_succ = rule_traffic_jam_menu_succ,
        .menu_str = rule_traffic_jam_menu_str,
        .reset = rule_traffic_jam_reset,
    },
    (rule) {
        .draw = rule_wolfram_draw,
        .init = rule_wolfram_init,
        .update = rule_wolfram_update,
        .menu_succ = rule_wolfram_succ,
        .menu_str = rule_wolfram_menu,
        .reset = rule_wolfram_reset_grid,
    },
    (rule) {
        .draw = rule_wolfram_draw,
        .init = rule_wolfram_init_random,
        .update = rule_wolfram_update,
        .menu_succ = rule_wolfram_succ,
        .menu_str = rule_wolfram_menu,
        .reset = rule_wolfram_reset_grid_random,
    },
};

void application_handle_input(application *app) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            app->keep_going = false;
            return;
        }
        if (e.type == SDL_KEYDOWN) {
            SDL_Keycode sym = e.key.keysym.sym;
            if (sym == SDLK_ESCAPE) {
                app->keep_going = false;
                return;
            } else if (sym == SDLK_n) {
                app->current_rule.update(app->simulation_state);
            } else if (sym == SDLK_m) {
                app->show_menu = !app->show_menu;
            } else if (sym == SDLK_SPACE) {
                app->paused = !app->paused;
            } else if (sym == SDLK_r) {
                srand(rand());
            } else if (sym == SDLK_h && app->show_menu) {
                menu_edit(&app->menu, &app->gc, false, app->simulation_state, app->current_rule.menu_succ, app->current_rule.menu_str);
            } else if (sym == SDLK_l && app->show_menu) {
                menu_edit(&app->menu, &app->gc, true, app->simulation_state, app->current_rule.menu_succ, app->current_rule.menu_str);
            } else if (sym == SDLK_j) {
                menu_scroll(&app->menu, false);
            } else if (sym == SDLK_k) {
                menu_scroll(&app->menu, true);
            } else if (sym >= SDLK_1 && sym <= SDLK_9) {
                app->menu.current_selection = 0;
                int i = sym - SDLK_1;
                if (i == app->current_rule_idx) {
                    app->current_rule.reset(app->simulation_state, app->xres, app->yres);
                } else {
                    if (i < len(rules)) {
                        app->current_rule = rules[i];
                        app->current_rule_idx = i;
                        free(app->simulation_state);
                        app->simulation_state = app->current_rule.init(app->xres, app->yres);
                        menu_clear(&app->menu);
                        menu_populate(&app->menu, &app->gc, app->simulation_state, app->current_rule.menu_str);
                    }
                }
            }
        }
    }
}

void application_draw(application *app) {
    gef_clear(&app->gc);

    app->current_rule.draw(&app->gc, app->simulation_state);
    if (app->show_menu) {
        menu_draw(&app->gc, &app->menu, 0, 0);
    }

    gef_present(&app->gc);
}

void application_step(application *app) {
    if (!app->paused) {
        app->current_rule.update(app->simulation_state);
    }
}


application application_init() {

    int xres = 640;
    int yres = 480;

    srand(1234567);

    const int start_rule_idx = 0;

    application app = (application) {
        .xres = xres,
        .yres = yres,
        .gc = gef_init("agent simulator", xres, yres, 60),
        .keep_going = true,
        .paused = true,
        .current_rule = rules[start_rule_idx],
        .current_rule_idx = start_rule_idx,
        .show_menu = false,
        .menu = {
            .current_selection = 0,
            .entries = {0},
            .font = gef_load_font("Hack-Regular.ttf", 24),
            .num_items = 0,
        },
    };


    app.simulation_state = app.current_rule.init(xres, yres);

    menu_populate(&app.menu, &app.gc, app.simulation_state, app.current_rule.menu_str);

    return app;
}