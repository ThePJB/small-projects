#include "application.h"
#include "SDL.h"
#include "predator_prey.h"

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
                app->update_rule(app->g);
            } else if (sym == SDLK_SPACE) {
                app->paused = !app->paused;
            }
        }
    }
}

void application_draw(application *app) {
    gef_clear(&app->gc);

    app->draw_rule(&app->gc, app->g);

    gef_present(&app->gc);
}

void application_step(application *app) {
    if (!app->paused) {
        app->update_rule(app->g);
    }
}

application application_init() {
    int xres = 640;
    int yres = 480;

    srand(1234567);

    application app =  (application) {
        .gc = gef_init("agent simulator", xres, yres, 60),
        .keep_going = true,
        .paused = true,

        .g = grid_init(sizeof(tile), xres, yres),

        .init_rule = rule_predator_prey_init,
        .update_rule = rule_predator_prey_update,
        .draw_rule = rule_predator_prey_draw,
    };

    app.init_rule(app.g);

    return app;
}