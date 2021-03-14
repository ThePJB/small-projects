#include <SDL.h>
#include <stdio.h>

#include "glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "vla.h"
#include "file_util.h"

#include "dankgl.h"
#include "mesh.h"
#include "pnc_vertex.h"
#include "camera.h"

#include "generation.h"

using glm::vec2;
using glm::vec3;
using glm::mat4;


void fatal(const char *msg) {
    printf("Fatal: %s\n", msg);
    exit(1);
}

int main(int argc, char** argv) {
    printf("make gc\n");
    dgl_context gc = dgl_context(1600, 900);

    printf("done gc\n");

    printf("make shaders\n");
    // load shader
    char *frag_str = slurp("pnc.frag");
    shader_id frag_id = gc.make_shader(frag_str, GL_FRAGMENT_SHADER);
    char *vert_str = slurp("pnc.vert");
    shader_id vert_id = gc.make_shader(vert_str, GL_VERTEX_SHADER);
    shader_pgm_id pnc_pgm = gc.make_shader_pgm(vert_id, frag_id);
    free(frag_str);
    free(vert_str);
    printf("done shaders\n");

    printf("make mesh\n");
    mesh<pnc_vertex> m = mesh<pnc_vertex>();

    /*
    printf("push triangle\n");
    m.push(pnc_vertex(vec3(0.5, 0, -1), vec3(0, 0, 1), vec3(1, 1, 1)));
    m.push(pnc_vertex(vec3(0, 1, -1), vec3(0, 0, 1), vec3(1, 1, 1)));
    m.push(pnc_vertex(vec3(1, 1, -1), vec3(0, 0, 1), vec3(1, 1, 1)));
    printf("upload...\n");
    m.upload();
    printf("done mesh\n");
    */

    printf("terrain time\n");
    generate_island(&m, 0.002, 69);
    m.model = mat4(
        10, 0, 0, 0,
        0, 10, 0, 0,
        0, 0, 10, 0,
        0, 0, 0, 1
    );

    m.upload();
    printf("terrain done\n");


    camera cam = camera(
        vec3(0, 2, 0),
        vec3(0, 1, -1)
    );
    
    
    float fovx = 100;

    bool running = true;

    while (running) {

        gc.check_errors();

        // handle input
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_MOUSEMOTION) {
                cam.look(e.motion.xrel, e.motion.yrel);
            }
        }
        
        float d = 0.01;

        // also poll input
        const uint8_t *state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_W]) cam.move(vec3(0, 0, -1), d);
        if (state[SDL_SCANCODE_A]) cam.move(vec3(-1, 0, 0), d);
        if (state[SDL_SCANCODE_S]) cam.move(vec3(0, 0, 1), d);
        if (state[SDL_SCANCODE_D]) cam.move(vec3(1, 0, 0), d);
        if (state[SDL_SCANCODE_LSHIFT]) cam.move(vec3(0, 1, 0), d);
        if (state[SDL_SCANCODE_LCTRL]) cam.move(vec3(0, -1, 0), d);




        // draw
        mat4 proj = glm::perspective(
            (float)DEG_TO_RAD*fovx,
            800.0f / 600.0f,
            0.1f,
            10000.0f
        );

        glClearColor(0.3, 0.5, 0.7, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m.draw(&gc, pnc_pgm, cam.view(), proj);
        SDL_GL_SwapWindow(gc.window);
    }
}