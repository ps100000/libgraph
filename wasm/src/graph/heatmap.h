#ifndef _HEATMAP_H_
#define _HEATMAP_H_
#include <memory>
#include <vector>

#ifdef __EMSCRIPTEN__
        #include <glm.hpp>
#else
        #include <glm/glm.hpp>
#endif

#include "gl/texture.h"
#include "gl/mesh.h"

class heatmap{
	private:
		struct series_metadata{
			float min;
			float max;
		};

		size_t series_buffer_offset;
		size_t series_count;
		mesh<gl_um_buffer<float>> surface;

		std::vector<series_metadata> series_meta;
		float min;
		float max;

		size_t get_index_count(size_t row_count);

		static inline std::shared_ptr<program> prog{};
		static inline std::shared_ptr<texture> gradient = nullptr;
	public:
		const size_t value_count;
		const size_t max_series_count;
		const size_t width;
		const size_t height;
		const glm::vec2 origin;

		heatmap();
		heatmap(
			size_t value_count,
			size_t max_series_count,
			size_t width,
			size_t height,
			glm::vec2 origin);

		size_t get_series_count();
		size_t get_buffer_offset();
		void add_series(std::vector<float> values);
		std::vector<float> get_series(size_t index);

		void draw();

		static void init();
};

#endif
