#include "application.h"
#include "SDL.h"
#include "aud.h"

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
            }
        }
    }
}

void application_draw(application *app) {
    
}

void application_init(application *app) {
    // todo sample rate is duplicated, how bad is that lol
    // i guess pa needs to know but my logic also needs to know
    
    app->mm = module_manager_init(48000);
    app->gc = gef_init("synth 2 electric boogaloo", 640, 480, 60);
    app->ac = aud_init(&app->mm, module_manager_rt_callback, 48000, CHUNK_SIZE);
    app->keep_going = true;
}