#include "application.h"
#include "util.h"
#include "coolmath.h"
#include "hash.h"

#include <stdio.h>



// glitched version with arcs of circles of varying levels of continuity makes cool icons
// i think from diff prime number seeds instead of same to get that

// this has a cool result. the polar coords is pretty neato
// im kind of projecting on my own interpretation of the heights of course

// maybe combining polar with cartesian is the go
// eg puting noise on for more variation to break it up a bit

// how to put polar into normal cartesian? generate polar provinces or mountains and glue them together

// next step maybe do jungle in regular 2d
// theres always heights and heightmap
// and rivers

// and caldera?
// and shallow water?

colour tile_colours[NUM_TILES];

int seed = 69;

// theta 0-2pi
float circ_noise(float theta, uint32_t seed, int bins) {

    float start_bin = theta / (2*M_PI) * bins;
    int start_bin_i = (int)start_bin;

    float f1 = hash_floatn(seed + start_bin_i * 2347234, 0, 1);
    float f2 = hash_floatn(seed + ((start_bin_i + 1) % bins)* 2347234, 0, 1);

    float t = cm_frac(start_bin);
    return cm_lerp(f1, f2, t*t*(3-2*t));
}

float circ_fbm3(float theta, uint32_t seed, int bins) {
    return (1.0000 * circ_noise(theta, seed, bins) +
            0.5000 * circ_noise(theta, seed + 435984, 2*bins) +
            0.2500 * circ_noise(theta, seed + 345345, 4*bins)) / 1.75;
}

float circ_fbm4(float theta, uint32_t seed, int bins) {
    return (1.0000 * circ_noise(theta, seed, bins) +
            0.5000 * circ_noise(theta, seed + 435984, 2*bins) +
            0.2500 * circ_noise(theta, seed + 345345, 4*bins) +
            0.1250 * circ_noise(theta, seed + 145345, 8*bins)) / 1.87;
}

void generate_level(grid g, int xres, int yres, int xo, int yo, int seed) {
    for (int i = 0; i < xres; i++) {
        for (int j = 0; j < yres; j++) {
            int x = i - xo;
            int y = j - yo;
            float xf = x + 0.5;
            float yf = y + 0.5;
            float r = sqrtf(xf*xf + yf*yf);
            float theta = atan2f(yf, xf) + M_PI; 

            float sea_r = 200 * circ_fbm4(theta, seed, 6);
            float sand_r = 30 * circ_fbm3(theta, seed*2309, 11);

            bool mountain = circ_noise(theta, seed*32453, 7) > 0.6;
            float mountain_r = 0.3 + 0.7 * circ_fbm3(theta, seed*23423, 5);
            mountain_r *= sea_r;
            float mountain_w = 4 + 30 * circ_fbm3(theta, seed*29804, 5);

            const float jungle_noise_scale = 0.015;

            tile t;
            if (r > sea_r) {
                t = T_WATER;
            } else {
                if (r < sea_r - sand_r) {
                    if (hash_fbm2_4(xf*jungle_noise_scale, yf*jungle_noise_scale, seed*23489723) > 0.6) {
                    //if (hash_noise2(xf*jungle_noise_scale, yf*jungle_noise_scale, seed*23489723) > 0.6) {
                        t = T_JUNGLE;
                    } else {
                        t = T_GRASS;
                    }        
                } else {
                    t = T_BEACH;
                }
            }
            
            if (mountain) {
                if ((r > mountain_r - mountain_w/2) && r < mountain_r + mountain_w/2) {
                    t = T_MOUNTAIN;
                }
            }
            grid_set(g, &t, i, j);
        }
    }    
}

void application_init(application *app, int xres, int yres) {
    tile_colours[T_BEACH] = gef_rgb(200,200,0);
    tile_colours[T_GRASS] = gef_rgb(0,200,0);
    tile_colours[T_JUNGLE] = gef_rgb(0,125,0);
    tile_colours[T_WATER] = gef_rgb(0,0,200);
    tile_colours[T_MOUNTAIN] = gef_rgb(200,200,200);

    app->gc = gef_init("island thing", xres, yres);
    app->keep_going = true;
    app->level = grid_init(sizeof(tile), xres, yres);

    generate_level(app->level, app->gc.xres, app->gc.yres, app->gc.xres/2, app->gc.yres/2, seed);
}

void application_update(application *app, double dt) {

}

void application_draw(application *app, double dt) {
    gef_clear(&app->gc);

    for (int i = 0; i < app->gc.xres; i++) {
        for (int j = 0; j < app->gc.yres; j++) {
            tile t;
            grid_get(app->level, &t, i, j);
            gef_draw_pixel(&app->gc, tile_colours[t], i, j);
        }
    }
    

    gef_present(&app->gc);
}

void application_handle_input(application *app) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            app->keep_going = false;
            return;
        } else if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_r) {
                seed = hash(seed);
                generate_level(app->level, app->gc.xres, app->gc.yres, app->gc.xres/2, app->gc.yres/2, seed);
            }
        }
    }
}