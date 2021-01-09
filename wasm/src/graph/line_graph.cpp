#include "line_graph.h"

line_graph::line_graph(
	size_t value_count,
	float max_y,
	int16_t width,
	int16_t height,
	glm::vec2 origin){
	std::vector<float> zero_init(value_count, 0);
	values = gl_um_buffer<float>::create(zero_init, GL_ARRAY_BUFFER);
	values->set(20000, 100);
	main_x_axis = x_axis(
		"x",
		width,
		0,
		value_count,
		(value_count / (width / 50)) / 5 * 5);
	main_x_axis.set_origin(origin);
	main_y_axis = y_axis(
		"y",
		height,
		0,
		max_y);
	main_y_axis.set_origin(origin);

	float inverse_x_scale = value_count * 1.0 / width;
	float y_scale =  height / max_y;

	GLint u_width = prog->get_uniform("width");
	GLint u_inverse_x_scale = prog->get_uniform("inverse_x_scale");
	GLint u_height = prog->get_uniform("height");
	GLint u_y_scale = prog->get_uniform("y_scale");
	GLint u_y_max = prog->get_uniform("y_max");
	GLint u_origin = prog->get_uniform("origin");
	GLint u_color = prog->get_uniform("color");
	std::function<void(std::shared_ptr<program>)> begin_func = [
		u_width,
		u_inverse_x_scale,
		u_height,
		u_y_scale,
		u_y_max,
		u_origin,
		u_color,
		max_y,
		origin,
		inverse_x_scale,
		y_scale](std::shared_ptr<program>){
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUniform1f(u_width, singleton<window_info>::value.w);
			glUniform1f(u_inverse_x_scale, inverse_x_scale);
			glUniform1f(u_height, singleton<window_info>::value.h);
			glUniform1f(u_y_scale, y_scale);
			glUniform1f(u_y_max, max_y);
			glUniform2f(u_origin, origin.x, origin.y);
			glUniform4f(u_color, 1, 0.125, 0.125, 0);
		};

	std::vector<GLuint> indices(0);
	lines = mesh<gl_um_buffer<float>>{
		values,
		indices,
		prog,
		{
			{
				"val",
				1,
				GL_FLOAT,
				sizeof(float)
			}
		},
		GL_LINE_STRIP,
		begin_func
	};

}

std::shared_ptr<gl_um_buffer<float>> line_graph::get_values_array(){
	return values;
}

void line_graph::init(){
	line_graph::prog = program::create("./resources/graph.v.glsl", "./resources/graph.f.glsl");
}

void line_graph::draw(){
	main_x_axis.draw();
	main_y_axis.draw();
	lines.draw();
}