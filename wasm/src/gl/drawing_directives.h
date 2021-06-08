#ifndef _DRAWING_DIRECTIVES_H_
#define _DRAWING_DIRECTIVES_H_
#include <memory>

#include <GL/glew.h>

#include "common/alignment.h"
#include "common/singleton.h"
#include "gl/text.h"
#include "gl/mesh_impl.h"
#include "gl/program.h"
#include "gl/window_info.h"

class drawing_directive{
	protected:
		mesh<gl_um_buffer<glm::vec2>> geometry;

		static inline std::shared_ptr<program> prog{};
	public:
		glm::vec2 pos;

		drawing_directive() = default;
		drawing_directive(glm::vec2 pos);

		static void init();
		void draw();
};

class rect: public drawing_directive {
	public:
		rect() = default;
		rect(glm::vec2 pos, uint16_t width, uint16_t height, SDL_Color color);
};

#endif