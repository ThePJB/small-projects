#include "application.h"
#include "SDL.h"

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
                world_step(&app->w, app->rule);
            } else if (sym == SDLK_SPACE) {
                app->paused = !app->paused;
            }
        }
    }
}

void application_draw(application *app) {
    gef_clear(&app->gc);

    world_draw(&app->w, &app->gc);

    gef_present(&app->gc);
}

void application_step(application *app) {
    if (!app->paused) {
        world_step(&app->w, app->rule);
    }
}

application application_init() {
    int xres = 640;
    int yres = 480;

    world w = world_init(xres, yres);

    // starting city
    int starting_x = xres/2;
    int starting_y = yres/2;

    world_add_entity(&w, (entity) {
        .alive = true,
        .carried_food = 0.9,
        .health = 1,
        .team = 0,
        .x = starting_x+5,
        .y = starting_y+5,
        .et = ENT_CITY,
    });

    for (int i = 0; i < 10; i++) {
        world_add_entity(&w, (entity) {
            .alive = true,
            .carried_food = 0,
            .et = ENT_DUDE,
            .team = 1,
            .x = starting_x,
            .y = starting_y,
        });
    }

    return (application) {
        .gc = gef_init("agent simulator", xres, yres, 60),
        .w = w,
        .keep_going = true,
        .paused = true,
        //.rule = world_rule_city,
        .rule = world_rule_predator_prey,
    };
}