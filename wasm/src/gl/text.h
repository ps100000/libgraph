#ifndef _TEXT_H_
#define _TEXT_H_

#include <string>
#include <map>
#include <memory>

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_ttf.h>
#ifdef __EMSCRIPTEN__
        #include <glm.hpp>
#else
        #include <glm/glm.hpp>
#endif

#include "program.h"
#include "gl_buffer.h"
#include "texture.h"
#include "mesh_impl.h"
#include "common/alignment.h"

class text{
    private:
        struct vertex{
            glm::vec2 coord;
            glm::vec2 tex_coord;
        };

        text(std::string message, SDL_Color color, uint32_t size, std::string font_path);

        std::shared_ptr<texture> tex;

        mesh<gl_um_buffer<vertex>> quad;
        static inline std::shared_ptr<program> prog{};
        int32_t w;
        int32_t h;

        static inline std::map<std::pair<std::string, int>, TTF_Font*> fonts{};
    public:
        const std::string message;
        const SDL_Color color;
        const uint32_t size;

        glm::vec2 pos = {0, 0};
        horizontal_align h_align = horizontal_align::left;
        vertical_align v_align = vertical_align::bottom;

        text(const text&) = delete;
        text(text&&) = delete;

        static std::shared_ptr<text> create(
            std::string message,
            SDL_Color color,
            uint32_t size,
            std::string font_path = "./resources/binary/DejaVuSansMono-Bold.ttf");
        static void init();

        void draw();
        int32_t get_width();
        int32_t get_height();
};

#endif
