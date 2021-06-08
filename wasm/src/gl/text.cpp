#include "text.h"

#include <iostream>

#include "common/singleton.h"
#include "capability.h"
#include "program.h"
#include "window_info.h"
#include "gl_buffer_impl.h"

text::text(std::string message, SDL_Color color, uint32_t size, std::string font_path): message(message), color(color), size(size){
	TTF_Font*& font = fonts[{font_path, size}];
	if(!font){
		font = TTF_OpenFont(font_path.c_str(), size);
		if (!font){
			std::cerr << "failed to open font!" << std::endl;
			assert(0);
		}
	}
	
	SDL_Surface* sFont = TTF_RenderText_Blended_Wrapped(font, message.c_str(), color, singleton<window_info>::value.w);
	assert(sFont);
	SDL_Surface* sFont2 = SDL_ConvertSurfaceFormat(sFont, SDL_PIXELFORMAT_ABGR8888, 0);
	assert(sFont2);
	w = sFont2->w;
	h = sFont2->h;

	std::vector<vertex> vertices = {
    	{{0,h},{0,1}},
		{{w,h},{1,1}},
    	{{w,0},{1,0}},
    	{{0,0},{0,0}}
    };

	std::vector<GLuint> indices = {
    	3,2,1,
    	  0,3
	};

	tex = texture::create(
			GL_RGBA,
			GL_RGBA,
			w,
			h,
			GL_UNSIGNED_BYTE,
			sFont2->pixels);

	GLint u_width = prog->get_uniform("width");
	GLint u_height = prog->get_uniform("height");
	GLint u_pos = prog->get_uniform("pos");
	GLint u_tex = prog->get_uniform("text_texture");

	quad = mesh<gl_um_buffer<vertex>>{
		gl_um_buffer<vertex>::create(vertices, GL_ARRAY_BUFFER),
		indices,
		prog,
		{
			{
				"a_coord",
				2,
				GL_FLOAT,
				sizeof(glm::vec2)
			},
			{
				"a_tex_coord",
				2,
				GL_FLOAT,
				sizeof(glm::vec2)
			}
		},
		GL_TRIANGLE_STRIP,
		(std::function<void(std::shared_ptr<program>)>)[this, u_width, u_height, u_pos, u_tex](std::shared_ptr<program>){
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glUniform1f(u_width, singleton<window_info>::value.w);
			glUniform1f(u_height, singleton<window_info>::value.h);
			glUniform2f(u_pos, pos.x - w * (int)h_align / 2, pos.y - h * (int)v_align / 2);
			glUniform1i(u_tex, *tex);
		}
	};

	SDL_FreeSurface(sFont);
	SDL_FreeSurface(sFont2);
}

std::shared_ptr<text> text::create(std::string message, SDL_Color color, uint32_t size, std::string font_path){
	return std::shared_ptr<text>(new text(message, color, size, font_path));
}

void text::init(){
	TTF_Init();
	text::prog = program::create("./resources/text.v.glsl", "./resources/text.f.glsl");
}

void text::draw(){
	static capability cap_depth{GL_DEPTH_TEST, GL_FALSE};
	static capability cap_blend{GL_BLEND, GL_TRUE};
	std::vector<std::reference_wrapper<context>> ctx = {
		*tex,
		cap_depth,
		cap_blend};

	context::use(ctx,(std::function<void()>)[this](){quad.draw();});
}

int32_t text::get_width(){
	return w;
}

int32_t text::get_height(){
	return h;
}
