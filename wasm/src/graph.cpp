#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <optional>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>

#include <GL/glew.h>
#include <SDL.h>

#include "common/util.h"
#include "common/singleton.h"
#include "gl/texture.h"
#include "gl/program.h"
#include "gl/gl_util.h"
#include "gl/text.h"
#include "gl/window_info.h"
#include "graph/axis.h"
#include "graph/line_graph.h"
#include "input/mouse_listener.h"
#include "input/click_listener.h"

#define GLM_FORCE_RADIANS

#ifdef __EMSCRIPTEN__
        #include <emscripten.h>
        #include <emscripten/bind.h>

        #include <glm.hpp>
        #include <gtc/matrix_transform.hpp>
        #include <gtc/type_ptr.hpp>
#else
        #include <glm/glm.hpp>
        #include <glm/gtc/matrix_transform.hpp>
        #include <glm/gtc/type_ptr.hpp>
#endif

bool debug = false;

struct graph_def{
    size_t value_count;
    float max_y;
    int16_t width;
    int16_t height;
    glm::vec2 origin;
};
uint16_t next_graph_id = 0;
std::map<uint16_t, line_graph> graphs;

uint16_t create_graph(graph_def def){
    graphs[next_graph_id] = {
        def.value_count,
        def.max_y,
        def.width,
        def.height,
        def.origin};
    return next_graph_id++;
}

bool set_graph_values(uint16_t id, uint16_t offset, std::vector<float> values){
    graphs.at(id).get_values_array()->set(offset, values);
    return true;
}

bool remove_graph(uint16_t id){
    return graphs.erase(id);
}

void set_window_size(uint16_t w, uint16_t h){
    if(
        singleton<window_info>::value.w != w
        || singleton<window_info>::value.h != h){
        singleton<window_info>::value.w = w;
        singleton<window_info>::value.h = h;
        SDL_SetWindowSize(singleton<window_info>::value.window, w, h);
        glViewport(0, 0, w, h);
    }
}

int init_resources(){
    texture::init();
    text::init();
    axis::init();
    line_graph::init();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

#ifndef __EMSCRIPTEN__
    glEnable(GL_LINE_SMOOTH);
#endif
    glEnable(GL_DEPTH_TEST);

    return 1;
}


void display(){
    glClearColor(1, 1, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(auto& [id, graph] : graphs){
        graph.draw();
    }
}

void keyDown(SDL_KeyboardEvent *ev){
    switch (ev->keysym.scancode){
        default:
            break;
    }
}

void windowEvent(SDL_WindowEvent *ev){
    switch(ev->event){
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            singleton<window_info>::value.w = ev->data1;
            singleton<window_info>::value.h = ev->data2;
            glViewport(0, 0, ev->data1, ev->data2);
            break;
        default:
            break;
    }
}

void mainLoop(void *window){
#ifndef __EMSCRIPTEN__
    while(1){
#endif
        display();
        SDL_GL_SwapWindow((SDL_Window*)window);
        SDL_Event ev;
        while (SDL_PollEvent(&ev)){
            switch (ev.type){
            case SDL_QUIT:
                return;
            case SDL_KEYDOWN:
                keyDown(&ev.key);
                break;
            case SDL_WINDOWEVENT:
                windowEvent(&ev.window);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                mouse_listener::publish_event(&ev.button);
                break;
            default:
                    break;
            }
        }
#ifndef __EMSCRIPTEN__
    }
#endif
}

int main(){
    SDL_Init(SDL_INIT_VIDEO);

#ifndef __EMSCRIPTEN__
    glEnable(GL_DEBUG_OUTPUT);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
#endif

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    singleton<window_info>::value.w = 640;
    singleton<window_info>::value.h = 480;
    SDL_Window *window = SDL_CreateWindow("",
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    singleton<window_info>::value.w, singleton<window_info>::value.h,
                    SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    singleton<window_info>::value.window = window;
    SDL_GLContext context = SDL_GL_CreateContext(window);
    GLenum glew_status = glewInit();

    if(GLEW_OK != glew_status){
            std::cerr << "Error: " << glewGetErrorString(glew_status) << std::endl;
            return 1;
    }

#ifdef __EMSCRIPTEN__
    if(!GLEW_VERSION_2_0){
            std::cerr << "No support for OpenGL 2.0 found" << std::endl;
            return 1;
    }
#else
    if(!GLEW_VERSION_3_2){
            std::cerr << "No support for OpenGL 3.2 found" << std::endl;
            return 1;
    }
#endif

    if(!init_resources())
            return EXIT_FAILURE;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(mainLoop, (void*)&window, 0, true);
#else
    SDL_GL_SetSwapInterval(1);
    mainLoop(window);

    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
#endif
    return EXIT_SUCCESS;
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::value_object<glm::vec2>("glm::vec2")
        .field("x", &glm::vec2::x)
        .field("y", &glm::vec2::y);

    emscripten::value_object<graph_def>("graph_def")
        .field("valueCount", &graph_def::value_count)
        .field("maxY", &graph_def::max_y)
        .field("width", &graph_def::width)
        .field("height", &graph_def::height)
        .field("origin", &graph_def::origin);

    emscripten::function("createGraph", &create_graph);
    emscripten::function("removeGraph", &remove_graph);
    emscripten::function("_setGraphValues", emscripten::optional_override(
        [](uint16_t id, unsigned long offset, unsigned long values, unsigned long value_count){
            std::vector<float> values_vector((float*)values,  (float*)values + value_count);
            return set_graph_values(id, offset, values_vector);
        }));
    emscripten::function("_setWindowSize", &set_window_size);
}
#endif