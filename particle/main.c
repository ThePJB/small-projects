#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <SDL.h>
#include <SDL_image.h>

#define true 1
#define false 0

typedef struct {
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *atlas;
    int xres;
    int yres;
    int zoom_factor;
} graphics;

#define die(X) printf("%s %d %s: dying -- %s\n", __FILE__, __LINE__, __func__, X), teardown()

void teardown();
graphics graphics_global;

graphics graphics_init(int xres, int yres, int zoom) {
    graphics g = {0};

    /* setup */
    if (SDL_Init(SDL_INIT_VIDEO) < 0) die("couldn't init sdl");

    g.window = SDL_CreateWindow("Best Twin Stick Shooter", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        xres, yres, SDL_WINDOW_SHOWN);
    if (g.window == NULL) die("couldn't create window");

    g.renderer = SDL_CreateRenderer(g.window, -1, SDL_RENDERER_ACCELERATED);
    if (g.renderer == NULL) die("couldn't create renderer");

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG);

    /* load assets */
    /*
    SDL_Surface* loaded_surface = IMG_Load("assets/atlas.png");
    g.atlas = SDL_CreateTextureFromSurface(g.renderer, loaded_surface);
    if (g.atlas == NULL) die("couldn't create texture");
    SDL_FreeSurface(loaded_surface);
    */

    g.xres = xres;
    g.yres = yres;
    g.zoom_factor = zoom;

    graphics_global = g;
    return g;
}

void teardown() {
    SDL_DestroyRenderer(graphics_global.renderer);
    SDL_DestroyWindow(graphics_global.window);
    IMG_Quit();
    SDL_Quit();
    exit(0);
}

uint64_t get_us() { 
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)t.tv_sec * 1000000 + (uint64_t)t.tv_nsec / 1000;
}

typedef enum {
    PT_AIR,
    PT_SAND,
    PT_WATER,
    NUM_PT,
} pixel_type;

typedef struct {

} pixel_properties;

#define xres 640
#define yres 480

void set_square(pixel_type *grid, int grid_w, int grid_h, int x, int y, int w, int h, pixel_type type) {
    for (int i = x; i < x + w; i++) {
        for (int j = y; j < y + h; j++) {
            if (i < 0 || i > grid_w || j < 0 || j > grid_h) continue;
            grid[j*grid_w + i] = type;            
        }
    }
}

int main(int argc, char** argv) { 
    graphics g = graphics_init(xres, yres, 1); 
    pixel_type grid[xres*yres] = {0};

    pixel_type selected_type = PT_SAND;

    int frame_period = 1000000/180;

    SDL_Event e;
    while (true) {
        int mouse_x = 0;
        int mouse_y = 0;

        uint64_t frame_start_us = get_us();
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) teardown();
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        teardown();
                        break;
                    case SDLK_1:
                        selected_type = PT_AIR;
                        break;
                    case SDLK_2:
                        selected_type = PT_SAND;
                        break;
                    case SDLK_3:
                        selected_type = PT_WATER;
                        break;
                }
            }
        }

        if (SDL_GetMouseState(&mouse_x, &mouse_y) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            int size = 16;
            set_square(grid, xres, yres, mouse_x - size/2, mouse_y - size/2, size, size, selected_type);
        }
            

        for (int n = 0; n < 3; n++) {

            /* update */
            for (int x = 0; x < xres; x++) {
                for (int y = yres; y > 0; y--) {
                    pixel_type *p, *p_below, *p_bl, *p_br, *p_l, *p_r;
                    p = &grid[y * xres + x];
                    if (*p == PT_SAND) {
                        if (y == yres) continue;
                        if (*(p_below = p + xres) == PT_AIR) {
                            *p_below = PT_SAND;
                            *p = PT_AIR;
                        } else if (*(p_bl = p + xres - 1) == PT_AIR) {
                            *p_bl = PT_SAND;
                            *p = PT_AIR;
                        } else if (*(p_br = p + xres + 1) == PT_AIR) {
                            *p_br = PT_SAND;
                            *p = PT_AIR;
                        }
                    }
                    if (*p == PT_WATER) {
                        if (y == yres) continue;
                        if (*(p_below = p + xres) == PT_AIR) {
                            *p_below = PT_WATER;
                            *p = PT_AIR;
                        } else if (*(p_bl = p + xres - 1) == PT_AIR) {
                            *p_bl = PT_WATER;
                            *p = PT_AIR;
                        } else if (*(p_br = p + xres + 1) == PT_AIR) {
                            *p_br = PT_WATER;
                            *p = PT_AIR;
                        } else if (*(p_r = p + 1) == PT_AIR) {
                            *p_r = PT_WATER;
                            *p = PT_AIR;
                        } else if (*(p_l = p - 1) == PT_AIR) {
                            *p_l = PT_WATER;
                            *p = PT_AIR;
                        }
                    }
                }
            }

        }

        /* draw */
        SDL_SetRenderDrawColor(g.renderer, 0,0,0,255);
        SDL_RenderClear(g.renderer);

        for (int x = 0; x < xres; x++) {
            for (int y = 0; y < yres; y++) {
                pixel_type p = grid[y * xres + x];
                if (p == PT_AIR) continue;
                if (p == PT_SAND) SDL_SetRenderDrawColor(g.renderer, 255,180,0,255);
                if (p == PT_WATER) SDL_SetRenderDrawColor(g.renderer, 100,100,255,255);
                SDL_RenderDrawPoint(g.renderer, x, y);
            }
        }

        SDL_RenderPresent(g.renderer);

        uint64_t frame_end_us = get_us();
        uint64_t delta_us = frame_end_us - frame_start_us;
        if (delta_us < frame_period) {
            usleep(frame_period - delta_us);
        }
        printf("frame time %f ms\n", delta_us / 1000.0);
    }
}
