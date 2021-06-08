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
#include "gl/drawing_directives.h"
#include "graph/axis.h"
#include "graph/label_axis.h"
#include "graph/status_axis.h"
#include "graph/line_graph.h"
#include "graph/heatmap.h"
#include "input/mouse_listener.h"
#include "input/click_listener.h"
#include "js_interface/js_graph.h"

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

static const uint16_t max_series = 4500;
static const uint16_t height_multi = 9;
static const uint16_t diagram_height = max_series * height_multi;

struct series_graph{
    size_t series_index;
    line_graph series;
    line_graph reference;
    x_axis x;
    y_axis y;
};

struct spectrum_meta{
    std::string reference_key;
    std::string timestamp;
    uint32_t part_number;
    std::string alarm_level;
    uint8_t workstep;
};
void add_spectrum_metadata(spectrum_meta meta);

glm::vec2 origin = {75, 50};
click_listener heatmap_listener{origin, 768 + 15, diagram_height, 0};
std::array<spectrum_meta, max_series> metadata;
std::map<std::string, std::vector<float>> references;
size_t next_metadata_index = 0;
std::shared_ptr<series_graph> series_view = nullptr;
std::shared_ptr<rect> backdrop = nullptr;
std::shared_ptr<heatmap> heat = nullptr;
std::array<std::shared_ptr<text>, 8> meta_text;
x_axis heat_x_axis;
label_axis steps_axis;
status_axis alarm_axis;

void create_series_view(size_t series_index, float y_series_graph){
    std::vector<float> series_values = heat->get_series(series_index);
    float max = -INFINITY;
    for(float value : series_values){
        if(value > max){
            max = value;
        }
    }
    if(y_series_graph > singleton<window_info>::value.h - 300){
        y_series_graph = singleton<window_info>::value.h - 300;
    }
    glm::vec2 series_origin = {origin.x, y_series_graph};

    backdrop = std::shared_ptr<rect>{new rect(
        series_origin + glm::vec2{-30, -50},
        768 + 30,
        250 + 50 + 50,
        {255, 255, 255, 128})};

    x_axis series_x_axis = x_axis(
        " ",
        768,
        0,
        1024,
        50,
        5,
        [](float val){
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1) << val * 12.5;
            return stream.str();
        });
    series_x_axis.set_origin(series_origin);
    y_axis series_y_axis = y_axis(
        "Amplitude(mG)",
        250,
        0,
        max,
        (max / 25 * 5));
    series_y_axis.set_origin(series_origin);

    series_view = std::shared_ptr<series_graph>(
        new series_graph{
            series_index,
            line_graph{
                1024,
                max,
                768,
                250,
                series_origin,
                {0, 255, 0, 0}
            },
            line_graph{
                1024,
                max,
                768,
                250,
                series_origin 
            },
            series_x_axis,
            series_y_axis});

    series_view->series.get_values_array()->set(0, series_values);
    spectrum_meta meta = metadata[(heat->get_buffer_offset() + series_index) % max_series];
    for(auto& [key, reference] : references){
        if(key == meta.reference_key){
            series_view->reference.get_values_array()->set(0, reference);
        }
    }

    /*meta_text[0] = text::create(
        "Uhrzeit:",
        {0, 0, 0, 0},
        18);
    meta_text[0]->pos = {50 + 768 + 25, y_series_graph + 235};
    meta_text[1] = text::create(
        meta.timestamp != "" ? meta.timestamp : "-",
        {0, 0, 0, 0},
        18);
    meta_text[1]->pos = {
        50 + 768 + 75,
        y_series_graph + 215};

    meta_text[2] = text::create(
        "Teilenummer:",
        {0, 0, 0, 0},
        18);
    meta_text[2]->pos = {50 + 768 + 25, y_series_graph + 185};
    meta_text[3] = text::create(
        std::to_string(meta.part_number),
        {0, 0, 0, 0},
        18);
    meta_text[3]->pos = {
        50 + 768 + 75,
        y_series_graph + 165};

    meta_text[4] = text::create(
        "Alarm-Level:",
        {0, 0, 0, 0},
        18);
    meta_text[4]->pos = {50 + 768 + 25, y_series_graph + 135};
    meta_text[5] = text::create(
        meta.alarm_level != "" ? meta.alarm_level : "-",
        {0, 0, 0, 0},
        18);
    meta_text[5]->pos = {
        50 + 768 + 75,
        y_series_graph + 115};

    meta_text[6] = text::create(
        "Arbeitsschritt:",
        {0, 0, 0, 0},
        18);
    meta_text[6]->pos = {50 + 768 + 25, y_series_graph + 85};
    meta_text[7] = text::create(
        std::to_string(meta.workstep),
        {0, 0, 0, 0},
        18);
    meta_text[7]->pos = {
        50 + 768 + 75,
        y_series_graph + 65};*/
}

int init_resources(){
    texture::init();
    text::init();
    axis::init();
    line_graph::init();
    heatmap::init();
    label_axis::init();
    status_axis::init();
    drawing_directive::init();

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

#ifndef __EMSCRIPTEN__
    glEnable(GL_LINE_SMOOTH);
#endif

    set_window_size(1024, 600);
    heat = std::shared_ptr<heatmap>(new heatmap{
        1024,
        max_series,
        768,
        diagram_height,
        origin});
    heat_x_axis = {
        "Frequenz(Hz)",
        768,
        0,
        1024,
        50,
        5,
        [](float val){
            std::stringstream stream;
            stream << std::fixed << std::setprecision(1) << val * 12.5;
            return stream.str();
        }};
    heat_x_axis.set_origin(origin);
    alarm_axis = {
        diagram_height,
        10,
        origin + glm::vec2{768 + 5, 0}
    };
    steps_axis = {
        diagram_height,
        768 + 15,
        origin + glm::vec2{768 + 15, 0}
    };

    heatmap_listener.on_click = [](SDL_MouseButtonEvent* const event){
        if(event->button == SDL_BUTTON_LEFT){
            size_t series_index = (event->y - origin.y) / height_multi;
            if(series_index < heat->get_series_count()){
                if(series_view && series_view->series_index == series_index){
                    series_view = nullptr;
                    backdrop = nullptr;
                    meta_text = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
                }else{
                    float y_series_graph = event->y;
                    create_series_view(series_index, y_series_graph);
                }
            }
        }};
    
    return 1;
}

void display(){
    set_window_size(
        1024,
        (heat->get_series_count() * height_multi) < 500 ?
            600
            : (heat->get_series_count() * height_multi) + 100);
    glClearColor(1, 1, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(auto& [id, graph] : graphs){
        graph.draw();
    }

    heat->draw();
    heat_x_axis.draw();
    alarm_axis.draw();
    steps_axis.draw();
    if(backdrop){
        backdrop->draw();
    }
    if(series_view){
        series_view->series.draw();
        series_view->reference.draw();
        series_view->x.draw();
        series_view->y.draw();
        for(auto& txt : meta_text){
            if(txt){
                txt->draw();
            }
        }
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
/*
    for (int i = 0; i < 20; ++i)
    {
    add_spectrum_metadata({
    "std::string reference_key",
    "std::string timestamp",
    1,
    "Unbekannt",
    2
});
    }
    for (int i = 0; i < 20; ++i)
    {
    add_spectrum_metadata({
    "1",
    "2",
    2,
    "3",
    0
});
    }
            std::vector<float> values_vector(1024 * 40);

    for (int i = 0; i < 40 * 1024; ++i) {
        values_vector.push_back(i);
    }
    heat->add_series(values_vector);*/

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


void add_spectrum_metadata(spectrum_meta meta){
    spectrum_meta last = metadata[(next_metadata_index - 1 + max_series) % max_series];
    std::string label;

    if(last.part_number != meta.part_number){
        label = "    Teil ";
        label += std::to_string(meta.part_number);

    }else if(last.workstep != meta.workstep){
        label = "Schritt ";
        label += std::to_string(meta.workstep);
    }
    
    static size_t last_offset = 0;
    steps_axis.shift(last_offset - heat->get_buffer_offset());
    
    float y = ((next_metadata_index - heat->get_buffer_offset() + max_series) % max_series) * height_multi;
    if(label.size()){
        label += " ";
        label += meta.timestamp;
        steps_axis.add_label(
            label,
            y - height_multi);
    }

    uint16_t level = 0;
    if(meta.alarm_level == "ok"){
        level = 1;
    }else if(meta.alarm_level == "warn"){
        level = 2;
    }else if(meta.alarm_level == "error"){
        level = 3;
    }

    alarm_axis.shift(last_offset - heat->get_buffer_offset());
    alarm_axis.add_state_change({level, y});

    metadata[next_metadata_index++] = meta;
    next_metadata_index %= max_series;
    last_offset = heat->get_buffer_offset();
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(my_module2) {
    emscripten::value_object<spectrum_meta>("spectrum_meta")
        .field("reference_key", &spectrum_meta::reference_key)
        .field("timestamp", &spectrum_meta::timestamp)
        .field("part_number", &spectrum_meta::part_number)
        .field("alarm_level", &spectrum_meta::alarm_level)
        .field("workstep", &spectrum_meta::workstep);

    emscripten::function("_addSpectrenValues", emscripten::optional_override(
        [](unsigned long values, unsigned long value_count){
            std::vector<float> values_vector((float*)values,  (float*)values + value_count);
            heat->add_series(values_vector);
        }));
    emscripten::function("_addReference", emscripten::optional_override(
        [](std::string key, unsigned long values, unsigned long value_count){
            std::vector<float> values_vector((float*)values,  (float*)values + value_count);
            references[key] = values_vector;
        }));
    emscripten::function("addSpectrumMetadata", &add_spectrum_metadata);
}
#endif
