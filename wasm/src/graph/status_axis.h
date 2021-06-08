#ifndef _STATUS_AXIS_H_
#define _STATUS_AXIS_H_

#include <memory>
#include <vector>

#include <SDL.h>
#ifdef __EMSCRIPTEN__
        #include <glm.hpp>
#else
        #include <glm/glm.hpp>
#endif

#include "gl/mesh_impl.h"
#include "gl/texture.h"

class status_axis{
	public:
		struct state_change{
			uint16_t state;
			float pos;
		};
	private:
		struct vertex{
			glm::vec2 pos;
			GLuint color_index;
		};
		mesh<gl_um_buffer<vertex>> rects;
		void generate_rects();

		std::vector<state_change> states;
		float length;
		float width;
		glm::vec2 origin;

		static inline std::shared_ptr<program> prog{};
		static const GLuint state_count = 4;
		static inline std::shared_ptr<texture> state_colors = nullptr;
	public:
		status_axis() = default;
		status_axis(float length, float width, glm::vec2 origin);

		void add_state_change(state_change change);
		void shift(float offset);

		void draw();

		static void init();
};

#endif
