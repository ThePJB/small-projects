#include "dankgl.h"

void dgl_fatal(const char *msg) {
    printf("DGL Fatal: %s\n", msg);
    exit(1);
}

dgl_context::dgl_context(int starting_xres, int starting_yres) {
    xres = starting_xres;
    yres = starting_yres;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        dgl_fatal("failed initing SDL");
    }

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // msaa
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    window = SDL_CreateWindow("island3d",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        xres, yres,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!window) {
        dgl_fatal("unable to create window");
    }

    gl = SDL_GL_CreateContext(window);

    printf("OpenGL loaded\n");
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));

    SDL_GL_SetSwapInterval(1); // vsync

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // if in fullscreen mode we need to ask res
    SDL_GetWindowSize(window, &xres, &yres);
    glViewport(0, 0, xres, yres);
    glClearColor(0.0f, 0.5f, 1.0f, 0.0f);
}

dgl_context::~dgl_context() {        
    SDL_GL_DeleteContext(gl);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

shader_id dgl_context::make_shader(char *program, unsigned int type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar * const*)&program, NULL);
    glCompileShader(shader);
    int success = 0;
    char buf[2048] = {0};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 2048, NULL, buf);
        printf("error compiling: %s\n", buf);
        dgl_fatal("error compiling shader");
    }
    return (shader_id)shader;
}

shader_pgm_id dgl_context::make_shader_pgm(shader_id vertex_shader, shader_id fragment_shader) {    
    int success;
    char buf[2048] = {0};
    shader_pgm_id shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 2048, NULL, buf);
        printf("error linking shaders: %s\n", buf);
        dgl_fatal("error linking shaders");
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return shader_program;
}

void dgl_context::check_errors() {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        printf("gl err %d\n", err);
    }
}