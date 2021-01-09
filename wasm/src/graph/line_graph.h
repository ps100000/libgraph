#ifndef _LINE_GRAPH_H_
#define _LINE_GRAPH_H_

#include <memory>

#include "gl/gl_buffer.h"
#include "gl/program.h"
#include "axis.h"

class line_graph{
	private:
		std::shared_ptr<gl_um_buffer<float>> values;
		x_axis main_x_axis;
		y_axis main_y_axis;
		mesh<gl_um_buffer<float>> lines;

		static inline std::shared_ptr<program> prog{};
	public:
		line_graph() = default;
		line_graph(
			size_t value_count,
			float max_y,
			int16_t width,
			int16_t height,
			glm::vec2 origin);

		std::shared_ptr<gl_um_buffer<float>> get_values_array();

		static void init();

		void draw();
};

#endif