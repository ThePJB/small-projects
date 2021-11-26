#include <SDL.h>
#include <stdbool.h>
#include "gef.h"
#include "game.h"

void frame(game *current_game) {
        gef_set_colour(0,0,0,255);
        gef_clear();
        game_draw(current_game);
        gef_present();
}

int main(int argc, char** argv) {

    // game settings
    int game_w = 12;
    int game_h = 12;
    int n_bombs = 40;
    
    gef_init();
    gef_set_res(720, 720);
    gef_set_frame_cap(60);
    gef_set_name("mineswep");
    gef_load_atlas("atlas.png");

    game current_game = game_init(n_bombs, game_w, game_h);
    int tile_w_px = gef_xres() / game_w;
    int tile_h_px = gef_yres() / game_h;

    bool keep_going = true;
    while(keep_going) {
        gef_start_frame();

        // Handle Input
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) keep_going = false;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        keep_going = false;
                        break;
                    case SDLK_r:
                        game_reset(&current_game, n_bombs, game_w, game_h);
                        break;
                }
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                int tile_x = e.button.x / tile_w_px;
                int tile_y = e.button.y / tile_h_px;

                if (e.button.button == SDL_BUTTON_RIGHT ||
                    e.button.button == SDL_BUTTON_LEFT) {

                    game_response resp = handle_tile_click(&current_game, tile_x, tile_y, 
                        e.button.button == SDL_BUTTON_LEFT);

                    if (resp == RESP_DEAD) {
                        printf("you lose nerd\n");
                    } else if (resp == RESP_VICTORY) {
                        printf("you win nerd\n");
                    }
                }
            }
        }

        frame(&current_game);

        gef_end_frame();
    }

    gef_teardown();
}

