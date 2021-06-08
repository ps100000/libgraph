#include "heatmap.h"

#include <cmath>
#include <cassert>

#include "common/singleton.h"
#include "gl/window_info.h"
#include "gl/mesh_impl.h"

size_t heatmap::get_index_count(size_t row_count){
	return (
			(value_count - 1) // anz. quadrate
			* 4 // anz. indices für ein Quadrat bzw. 2 Dreiecke
			+ 3 // "Link" zur nächsten Reihe
		)
		* (row_count - 1); // anz. Quadrat-Reihen
}

heatmap::heatmap():
		value_count(0),
		max_series_count(0),
		width(0),
		height(0),
		origin({0,0}){}

heatmap::heatmap(
	size_t value_count,
	size_t max_series_count,
	size_t width,
	size_t height,
	glm::vec2 origin):
		series_buffer_offset(0),
		series_count(0),
		series_meta(max_series_count, {INFINITY, -INFINITY}),
		min(INFINITY),
		max(-INFINITY),
		value_count(value_count),
		max_series_count(max_series_count),
		width(width),
		height(height),
		origin(origin){
	GLint u_min_val = prog->get_uniform("min_val");
	GLint u_max_val = prog->get_uniform("max_val");
	GLint u_value_count = prog->get_uniform("value_count");
	GLint u_series_count = prog->get_uniform("series_count");
	GLint u_series_buffer_offset = prog->get_uniform("series_buffer_offset");
	GLint u_width = prog->get_uniform("width");
	GLint u_height = prog->get_uniform("height");
	GLint u_origin = prog->get_uniform("origin");
	GLint u_window_width = prog->get_uniform("window_width");
	GLint u_window_height = prog->get_uniform("window_height");
	GLint u_gradient = prog->get_uniform("gradient");

	std::function<void(std::shared_ptr<program>)> begin_func = 
		[=](std::shared_ptr<program>){
			glUniform1f(u_min_val, min);
			glUniform1f(u_max_val, max);
			glUniform1i(u_value_count, value_count);
			//glUniform1i(u_series_count, series_count); // skalliert y mit num. serien
			glUniform1i(u_series_count, max_series_count);
			glUniform1i(u_series_buffer_offset, series_buffer_offset);
			glUniform1f(u_width, width);
			glUniform1f(u_height, height);
			glUniform2f(u_origin, origin.x, origin.y);
			glUniform1f(u_window_width, singleton<window_info>::value.w);
			glUniform1f(u_window_height, singleton<window_info>::value.h);
			glUniform1i(u_gradient, *gradient);
		};

	std::vector<float> zero_init(max_series_count * value_count, 0);
	std::shared_ptr<gl_um_buffer<float>> values = gl_um_buffer<float>::create(zero_init, GL_ARRAY_BUFFER);

	size_t indices_count = get_index_count(max_series_count + 1);
	std::vector<GLuint> indices(indices_count);

	size_t index = 0;
	for(size_t i = 0; i < max_series_count - 1; i++){
		size_t row_begin = i * value_count;
		for(size_t j = 0; j < value_count - 1; j++){
			indices[index++] = row_begin + j + value_count;
			indices[index++] = row_begin + j + value_count + 1;
			indices[index++] = row_begin + j + 0;
			indices[index++] = row_begin + j + 1;
		}
		indices[index++] = row_begin + value_count - 1;
		indices[index++] = row_begin + value_count;
		indices[index++] = row_begin + value_count;
	}

	// add row that will complete the loop
	size_t connect_row_begin = (max_series_count - 1) * value_count;
	for(size_t j = 0; j < value_count - 1; j++){
		indices[index++] = j;
		indices[index++] = j + 1;
		indices[index++] = connect_row_begin + j + 0;
		indices[index++] = connect_row_begin + j + 1;
	}
	indices[index++] = connect_row_begin + value_count - 1;
	indices[index++] = 0;
	indices[index++] = 0;
		
	assert(index == indices_count);

	surface = mesh<gl_um_buffer<float>>{
		values,
		indices,
		prog,
		{
			{
				"i_val",
				1,
				GL_FLOAT,
				sizeof(float)
			}
		},
		GL_TRIANGLE_STRIP,
		begin_func
		};
}


size_t heatmap::get_series_count(){
	return series_count;
}

size_t heatmap::get_buffer_offset(){
	return series_buffer_offset;
}

void heatmap::add_series(std::vector<float> values){
	assert(!(values.size() % value_count));
	size_t start = series_count < max_series_count ?
		series_count : series_buffer_offset;

	size_t end = start;
	for (size_t i = 0; i < values.size() / value_count; i++){
		size_t index;
		if(series_count < max_series_count){
			index = series_count++;
		}else{
			index = series_buffer_offset++;
			series_buffer_offset %= max_series_count;
		}
		end = index;

		float series_min = INFINITY;
		float series_max = -INFINITY;
		for (size_t j = i * value_count; j < (i + 1) * value_count; j++){
			if(series_min > values[j]){
				series_min = values[j];
			}
			if(series_max < values[j]){
				series_max = values[j];
			}
		}
		series_meta[index] = {series_min, series_max};

	}

	end = (end + 1) % max_series_count;
	if(end > start){
		surface.vertex_buffer->set(
			start * value_count,
			values.data(),
			(end - start) * value_count);
	}else{
		size_t first_part_length = (max_series_count - start) * value_count;
		surface.vertex_buffer->set(
			start * value_count,
			values.data(),
			first_part_length);
		surface.vertex_buffer->set(
			0,
			values.data() + first_part_length,
			end * value_count);
	}

	float new_min = INFINITY;
	float new_max = -INFINITY;

	for(auto& meta : series_meta){
		if(new_min > meta.min){
			new_min = meta.min;
		}
		if(new_max < meta.max){
			new_max = meta.max;
		}
	}

	min = new_min;
	max = new_max;
}

std::vector<float> heatmap::get_series(size_t index){
	return surface.vertex_buffer->get(((index + series_buffer_offset) % max_series_count) * value_count, value_count);
}

void heatmap::draw(){
	gradient->use([this](){
		if(series_count < 2){
			return;
		}
		if(series_count < max_series_count){
			surface.draw(0, get_index_count(series_count));
		}else{
			surface.draw(
				0,
				get_index_count((max_series_count + series_buffer_offset - 1) % max_series_count));
			surface.draw(
				get_index_count(series_buffer_offset + 1),
				get_index_count(max_series_count - series_buffer_offset + 1));
		}
	});
}

void heatmap::init(){
	heatmap::prog = program::create("./resources/heatmap.v.glsl", "./resources/heatmap.f.glsl");
	
	GLubyte gradient_colors[1][11 * GL_RGB] = {{
        0x31, 0x36, 0x95,
        0x45, 0x75, 0xb4,
        0x74, 0xad, 0xd1,
        0xab, 0xd9, 0xe9,
        0xe0, 0xf3, 0xf8,
        0xff, 0xff, 0xbf,
        0xfe, 0xe0, 0x90,
        0xfd, 0xae, 0x61,
        0xf4, 0x6d, 0x43,
        0xd7, 0x30, 0x27,
        0xa5, 0x00, 0x26}};
	heatmap::gradient = texture::create(
    	GL_RGB,
    	GL_RGB,
    	11,
    	1,
    	GL_UNSIGNED_BYTE,
    	gradient_colors);
}
