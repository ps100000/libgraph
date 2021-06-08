#include "status_axis.h"

#include "gl/window_info.h"
#include "common/singleton.h"

void status_axis::generate_rects(){
	std::vector<vertex> vertices(states.size() * 4);
	std::vector<GLuint> indices(states.size() * 6);
	if(states.size()){
		size_t vertices_index = 0;
		size_t indices_index = 0;
			
		float previous = 0;
		for(std::vector<state_change>::iterator state = states.begin(); state != states.end(); state++){
			size_t offset = vertices_index;

			vertices[vertices_index++] = {{0    , previous}, state->state};
			vertices[vertices_index++] = {{width, previous}, state->state};
			vertices[vertices_index++] = {{0    , state->pos}, state->state};
			vertices[vertices_index++] = {{width, state->pos}, state->state};
			previous = state->pos;

			indices[indices_index++] = offset + 0;
			indices[indices_index++] = offset + 1;
			indices[indices_index++] = offset + 2;

			indices[indices_index++] = offset + 2;
			indices[indices_index++] = offset + 3;
			indices[indices_index++] = offset + 1;
		}
	}

	GLint u_width = prog->get_uniform("width");
	GLint u_height = prog->get_uniform("height");
	GLint u_origin = prog->get_uniform("origin");
	GLint u_color_map = prog->get_uniform("color_map");
	glm::vec2 orig = origin;
	std::function<void(std::shared_ptr<program>)> begin_func = [
		u_width,
		u_height,
		u_origin,
		u_color_map,
		orig](std::shared_ptr<program>){
			glUniform1f(u_width, singleton<window_info>::value.w);
			glUniform1f(u_height, singleton<window_info>::value.h);
			glUniform2f(u_origin, orig.x, orig.y);
			glUniform1i(u_color_map, *state_colors);
		};

	rects = mesh<gl_um_buffer<vertex>>{
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
				"color_index",
				1,
				GL_INT,
				sizeof(GLuint)
			}
		},
		GL_TRIANGLES,
		begin_func
	};
}

status_axis::status_axis(float length, float width, glm::vec2 origin):
	length(length),
	width(width),
	origin(origin){
	generate_rects();
}

void status_axis::add_state_change(state_change change){
	auto insert_iter = std::find_if(
		states.begin(),
		states.end(),
		[change](state_change a){
			return a.pos > change.pos;
		});
	if(insert_iter != states.begin() && (insert_iter - 1)->pos == change.pos){
		(insert_iter - 1)->state = change.state;
	}else{
		states.insert(insert_iter,change);
	}
	generate_rects();
}

void status_axis::shift(float offset){
	std::vector<std::vector<state_change>::iterator> to_remove;
	for(std::vector<state_change>::iterator state = states.begin(); state != states.end(); state++){
		float y = state->pos + offset;
		if(y < 0 || y > length){
			to_remove.push_back(state);
		}else{
			state->pos = y;
		}
	}
	for(auto state = to_remove.rbegin(); state != to_remove.rend(); state++){
		states.erase(*state);
	}
	generate_rects();
}

void status_axis::draw(){
	state_colors->use([this](){rects.draw();});
}

void status_axis::init(){
	GLubyte state_colors[1][state_count * GL_RGB] = {{
		0x40, 0x40, 0x40,
		0x00, 0x89, 0x3d,
		0xfa, 0xc8, 0x32,
		0xaa, 0x0a, 0x05
    }};
	status_axis::state_colors = texture::create(
    	GL_RGB,
    	GL_RGB,
    	state_count,
    	1,
    	GL_UNSIGNED_BYTE,
    	state_colors);
	status_axis::prog = program::create("./resources/simple_color.v.glsl", "./resources/simple_color.f.glsl");
}


