#ifndef _AXIS_H_
#define _AXIS_H_
#include <memory>
#include <sstream>
#include <iomanip>

#include <GL/glew.h>

#include "common/alignment.h"
#include "common/singleton.h"
#include "gl/text.h"
#include "gl/mesh_impl.h"
#include "gl/program.h"
#include "gl/window_info.h"

class axis{
	protected:
		mesh<gl_um_buffer<glm::vec2>> lines;
		mesh<gl_um_buffer<glm::vec2>> heads;
		std::vector<std::shared_ptr<text>> seg_labels;
		std::shared_ptr<text> descr_text;

		float seg_width;
		uint8_t nr_subdiv;
		float min;
		float max;
		uint16_t length;
		uint16_t neg_length;
		uint16_t pos_length;
		glm::vec2 origin;
		std::function<std::string(float)> label_func;

		static inline std::shared_ptr<program> prog{};
		char direction;
		
		void update();

		inline glm::vec2 swap_if(glm::vec2 in, bool swap);
	public:
		axis() = default;
		axis(
			std::string description,
			uint16_t length,
			float min,
			float max,
			float seg_width,
			uint8_t nr_subdiv,
			char direction,
			std::function<std::string(float)> label_func);

		void set_segment_width(float width, uint8_t nr_subdivisions = 5);
		void set_min(float min_val);
		void set_max(float max_val);
		virtual void set_origin(glm::vec2 origin_val);

		static void init();
		static const inline std::function<std::string(float)> std_label_func =
			[](float val){
				std::stringstream stream;
				stream << std::fixed << std::setprecision(1) << val;
				return stream.str();
			};

		void draw();
};

class x_axis: public axis{
	public:
		x_axis() = default;

		x_axis(
			std::string description,
			uint16_t length,
			float min,
			float max,
			float seg_width = 10,
			uint8_t nr_subdiv = 5,
			std::function<std::string(float)> label_func = std_label_func);
};

class y_axis: public axis{
	public:
		y_axis() = default;

		y_axis(
			std::string description,
			uint16_t length,
			float min,
			float max,
			float seg_width = 10,
			uint8_t nr_subdiv = 5,
			std::function<std::string(float)> label_func = std_label_func);
};

#endif