#ifndef _JS_GRAPH_H_
#define _JS_GRAPH_H_

#include <map>
#include <stdint.h>

#ifdef __EMSCRIPTEN__
        #include <glm.hpp>
#else
        #include <glm/glm.hpp>
#endif

#include "graph/line_graph.h"

struct graph_def{
    size_t value_count;
    float max_y;
    int16_t width;
    int16_t height;
    glm::vec2 origin;
};
extern std::map<uint16_t, line_graph> graphs;

uint16_t create_graph(graph_def def);
bool set_graph_values(uint16_t id, uint16_t offset, std::vector<float> values);
bool remove_graph(uint16_t id);

void set_window_size(uint16_t w, uint16_t h);

#endif
