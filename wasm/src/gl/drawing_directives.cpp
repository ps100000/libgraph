#include "drawing_directives.h"

drawing_directive::drawing_directive(glm::vec2 pos): pos(pos){}

void drawing_directive::init(){
	drawing_directive::prog = program::create("./resources/axis.v.glsl", "./resources/single_color.f.glsl");
}

void drawing_directive::draw(){
	geometry.draw();
	glDisable(GL_BLEND);
}

/**************************************************************/

rect::rect(glm::vec2 pos, uint16_t width, uint16_t height, SDL_Color color): drawing_directive(pos){
	std::vector<glm::vec2> vertices(4);
	vertices[0] = {0    , 0};
	vertices[1] = {0    , height};
	vertices[2] = {width, 0};
	vertices[3] = {width, height};
	std::vector<GLuint> indices(4);
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	GLint u_width = drawing_directive::prog->get_uniform("width");
	GLint u_height = drawing_directive::prog->get_uniform("height");
	GLint u_origin = drawing_directive::prog->get_uniform("origin");
	GLint u_color = drawing_directive::prog->get_uniform("color");
	std::function<void(std::shared_ptr<program>)> begin_func = 
		[u_width, u_height, u_origin, u_color, pos, color](std::shared_ptr<program>){
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUniform1f(u_width, singleton<window_info>::value.w);
			glUniform1f(u_height, singleton<window_info>::value.h);
			glUniform2f(u_origin, pos.x, pos.y);
			glUniform4f(u_color, color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
		};

	geometry = mesh<gl_um_buffer<glm::vec2>>{
		gl_um_buffer<glm::vec2>::create(vertices, GL_ARRAY_BUFFER),
		indices,
		drawing_directive::prog,
		{
			{
				"a_coord",
				2,
				GL_FLOAT,
				sizeof(glm::vec2)
			}
		},
		GL_TRIANGLE_STRIP,
		begin_func
	};
}
