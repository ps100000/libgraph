#include <string>
#include <map>

extern const std::map<std::string, std::string> resource2c = {
    {"./resources/axis.f.glsl",
        "out vec4 color;\n"
        "\n"
        "void main(void) {\n"
        "	color = vec4(0, 0, 0, 0);\n"
        "}\n"
        ""},
    {"./resources/axis.v.glsl",
        "in vec2 a_coord;\n"
        "uniform float width;\n"
        "uniform float height;\n"
        "uniform vec2 origin;\n"
        "\n"
        "void main(void) {\n"
        "	vec2 true_coord = a_coord + origin;\n"
        "	gl_Position = vec4(true_coord.x / width - 0.5, true_coord.y / height - 0.5, 0, 0.5);\n"
        "}\n"
        ""},
    {"./resources/graph.f.glsl",
        "uniform vec4 color;\n"
        "out vec4 color_out;\n"
        "\n"
        "void main(void) {\n"
        "	color_out = color;\n"
        "}\n"
        ""},
    {"./resources/graph.v.glsl",
        "in float val;\n"
        "uniform float width;\n"
        "uniform float inverse_x_scale;\n"
        "uniform float height;\n"
        "uniform float y_scale;\n"
        "uniform float y_max;\n"
        "uniform vec2 origin;\n"
        "\n"
        "void main(void) {\n"
        "	float value = val < y_max ? val : y_max;\n"
        "	gl_Position = vec4(\n"
        "		(float(gl_VertexID) / inverse_x_scale + origin.x) / width - 0.5,\n"
        "		(value * y_scale + origin.y) / height - 0.5,\n"
        "		0,\n"
        "		0.5);\n"
        "}\n"
        ""},
    {"./resources/text.f.glsl",
        "in vec2 tex_coord;\n"
        "uniform sampler2D text_texture;\n"
        "out vec4 color;\n"
        "\n"
        "void main(void) {\n"
        "	color = texture(text_texture, tex_coord);\n"
        "}\n"
        ""},
    {"./resources/text.v.glsl",
        "in vec2 a_coord;\n"
        "in vec2 a_tex_coord;\n"
        "out vec2 tex_coord;\n"
        "uniform float width;\n"
        "uniform float height;\n"
        "uniform vec2 pos;\n"
        "\n"
        "void main(void) {\n"
        "	tex_coord = vec2(a_tex_coord.x, 1.0-a_tex_coord.y);\n"
        "	vec2 true_coord = a_coord + pos;\n"
        "	gl_Position = vec4(true_coord.x / width - 0.5, true_coord.y / height - 0.5, 0, 0.5);\n"
        "}\n"
        ""}
};
