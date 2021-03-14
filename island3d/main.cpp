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

using glm::vec3;
using glm::mat4;


void fatal(const char *msg) {
    printf("Fatal: %s\n", msg);
    exit(1);
}

int main(int argc, char** argv) {
    printf("make gc\n");
    dgl_context gc = dgl_context(800, 600);

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
    printf("push triangle\n");
    m.push(pnc_vertex(vec3(0.5, 0, -1), vec3(0, 0, 1), vec3(1, 1, 1)));
    m.push(pnc_vertex(vec3(0, 1, -1), vec3(0, 0, 1), vec3(1, 1, 1)));
    m.push(pnc_vertex(vec3(1, 1, -1), vec3(0, 0, 1), vec3(1, 1, 1)));
    printf("upload...\n");
    m.upload();
    printf("done mesh\n");


    // now do the meshinery and draw a triangle
    
    float fovx = 100;

    vec3 cam_pos = vec3(0, 0, 0);
    vec3 cam_up = vec3(0, 1, 0);
    vec3 cam_front = vec3(0, 0, -1);


    bool running = true;

    while (running) {

        gc.check_errors();

        // handle input
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }
        // draw
        mat4 view = glm::lookAt(
            cam_pos,
            vec3(0.5, 0, -1),
            cam_up
        );

        mat4 proj = glm::perspective(
            fovx,
            800.0f / 600.0f,
            0.1f,
            10000.0f
        );

        glClearColor(0.3, 0.5, 0.7, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m.draw(&gc, pnc_pgm, view, proj);
        SDL_GL_SwapWindow(gc.window);
    }
}