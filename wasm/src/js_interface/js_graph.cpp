#include "js_graph.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <emscripten/html5.h>
#endif

static uint16_t next_graph_id = 0;
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
#ifdef __EMSCRIPTEN__
        emscripten_set_canvas_element_size("#canvas", w, h);
#endif
        singleton<window_info>::value.w = w;
        singleton<window_info>::value.h = h;
        SDL_SetWindowSize(singleton<window_info>::value.window, w, h);
        glViewport(0, 0, w, h);
    }
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