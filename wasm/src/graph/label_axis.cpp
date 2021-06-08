#include "label_axis.h"

#include "gl/window_info.h"
#include "common/singleton.h"

void label_axis::generate_lines(){
	std::vector<glm::vec2> vertices(labels.size() * 2);
	std::vector<GLuint> indices(0);
	size_t index = 0;
	for(auto& label : labels){
		vertices[index++] = {
				-line_length,
				label->pos.y - origin.y
			};
		vertices[index++] = {
				10,
				label->pos.y - origin.y
			};
	}

	GLint u_width = prog->get_uniform("width");
	GLint u_height = prog->get_uniform("height");
	GLint u_origin = prog->get_uniform("origin");
	glm::vec2 orig = origin;
	std::function<void(std::shared_ptr<program>)> begin_func = 
		[u_width, u_height, u_origin, orig](std::shared_ptr<program>){
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUniform1f(u_width, singleton<window_info>::value.w);
			glUniform1f(u_height, singleton<window_info>::value.h);
			glUniform2f(u_origin, orig.x, orig.y);
		};

	lines = mesh<gl_um_buffer<glm::vec2>>{
		gl_um_buffer<glm::vec2>::create(vertices, GL_ARRAY_BUFFER),
		indices,
		prog,
		{
			{
				"a_coord",
				2,
				GL_FLOAT,
				sizeof(glm::vec2)
			}
		},
		GL_LINES,
		begin_func
	};
}

label_axis::label_axis(float length, float line_length, glm::vec2 origin):
	length(length),
	line_length(line_length),
	origin(origin){
	generate_lines();
}

void label_axis::init(){
	label_axis::prog = program::create("./resources/axis.v.glsl", "./resources/axis.f.glsl");
}

void label_axis::add_label(std::string label, float pos){
	std::shared_ptr<text> label_txt = text::create(
        label,
        {0, 0, 0, 255},
        12);
	label_txt->pos = {origin.x + 10, origin.y + pos};
	label_txt->v_align = vertical_align::center;
	labels.push_back(label_txt);
	generate_lines();
}

void label_axis::shift(float offset){
	std::vector<std::vector<std::shared_ptr<text>>::iterator> to_remove;
	for(auto label = labels.begin(); label != labels.end(); label++){
		float y = label->get()->pos.y + offset;
		float rel_y = y - origin.y;
		if(rel_y < 0 || rel_y > length){
			to_remove.push_back(label);
		}else{
			label->get()->pos.y = y;
		}
	}
	for(auto label = to_remove.rbegin(); label != to_remove.rend(); label++){
		labels.erase(*label);
	}
	generate_lines();
}

void label_axis::draw(){
	glLineWidth(2);
	lines.draw();
	glLineWidth(1);
	for(auto& label : labels){
		label->draw();
	}
}
