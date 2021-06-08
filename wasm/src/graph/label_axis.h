#ifndef _LABEL_AXIS_H_
#define _LABEL_AXIS_H_
#include <vector>

#include "gl/mesh_impl.h"
#include "gl/texture.h"
#include "gl/text.h"

class label_axis{
	private:
		std::vector<std::shared_ptr<text>> labels;
		mesh<gl_um_buffer<glm::vec2>> lines;
		float length;
		float line_length;
		glm::vec2 origin;

		static inline std::shared_ptr<program> prog{};

		void generate_lines();
	public:
		label_axis() = default;
		label_axis(float length, float line_length, glm::vec2 origin);

		static void init();
		void add_label(std::string label, float pos);
		void shift(float offset);
		void draw();
};

#endif
