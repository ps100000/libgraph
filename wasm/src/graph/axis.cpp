#include "axis.h"

void axis::update(){
	bool is_y = (direction == 'y');
	float span = max - min;
	uint32_t nr_segs = span / seg_width;
	uint32_t nr_points = 2 + ((nr_segs + 2) * (nr_subdiv > 1 ? nr_subdiv : 1)) * 2;

	pos_length = length * max / span;
	neg_length = length * (-min) / span;

	std::vector<glm::vec2> vertices(nr_points);

	vertices[0] = swap_if({min * length / span, 0}, is_y);
	vertices[1] = swap_if({max * length / span, 0}, is_y);
	uint32_t index = 2;

	float seg_length = length / span * seg_width;
	for(int s = min / seg_width; s < max / seg_width; s++){
		float x = s * seg_length;
		vertices[index++] = swap_if({x,0}, is_y);
		vertices[index++] = swap_if({x,-10}, is_y);
		for(int sub = 1; sub < nr_subdiv; sub++){
			float x_sub = x + sub * seg_length / nr_subdiv;
			if(x_sub < neg_length){
				continue;
			}
			if(x_sub > pos_length){
				break;
			}
			vertices[index++] = swap_if({x_sub,0}, is_y);
			vertices[index++] = swap_if({x_sub,-5}, is_y);
		}

		std::shared_ptr<text> seg_label = text::create(label_func(s * seg_width), {0, 0, 0, 0}, 12);
		if(!is_y){
		    seg_label->h_align = horizontal_align::center;
		    seg_label->v_align = vertical_align::top;
		    if((seg_label->get_width() > seg_length * 0.8) && (s & 1)){
			    seg_label->pos = origin + glm::vec2{x, -25};
		    }else{
			    seg_label->pos = origin + glm::vec2{x, -10};
			}
		}else{
		    seg_label->h_align = horizontal_align::right;
		    seg_label->v_align = vertical_align::center;
		    seg_label->pos = origin + glm::vec2{-10, x};	
		}
		seg_labels.push_back(seg_label);
	}
	vertices.resize(index);
	std::vector<GLuint> indices(0);

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

	if(is_y){
    	descr_text->v_align = vertical_align::bottom;
	    descr_text->pos = origin + glm::vec2{5, pos_length};
	}else{
	    descr_text->v_align = vertical_align::top;
	    descr_text->pos = origin + glm::vec2{pos_length + 12.5, -5};
	}

    vertices = std::vector<glm::vec2>();
    if(pos_length > 0){
    	vertices.push_back(swap_if({pos_length, 3.5}, is_y));
    	vertices.push_back(swap_if({pos_length + 10, 0}, is_y));
    	vertices.push_back(swap_if({pos_length, -3.5}, is_y));
    }
    if(neg_length > 0){
    	vertices.push_back(swap_if({-neg_length, 3.5}, is_y));
    	vertices.push_back(swap_if({-neg_length - 10, 0}, is_y));
    	vertices.push_back(swap_if({-neg_length, -3.5}, is_y));
    }
	
	heads = mesh<gl_um_buffer<glm::vec2>>{
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
		GL_TRIANGLES,
		begin_func
	};
}

glm::vec2 axis::swap_if(glm::vec2 in, bool swap){
	return (swap ? glm::vec2{in.y, in.x} : in);
}

axis::axis(
	std::string description,
	uint16_t length,
	float min,
	float max,
	float seg_width,
	uint8_t nr_subdiv,
	char direction,
	std::function<std::string(float)> label_func):
		seg_width(seg_width),
		nr_subdiv(nr_subdiv),
		min(min),
		max(max),
		length(length),
		origin(0,0),
		label_func(label_func),
		direction(direction) {
	descr_text = text::create(description, {0, 0, 0, 0}, 16);
    descr_text->h_align = horizontal_align::left;
}

void axis::init(){
	axis::prog = program::create("./resources/axis.v.glsl", "./resources/axis.f.glsl");
}

void axis::set_segment_width(float width, uint8_t nr_subdivisions){
	if(seg_width != width || nr_subdiv != nr_subdivisions){
		seg_width = width;
		nr_subdiv = nr_subdivisions;
		update();
	}
}

void axis::set_min(float min_val){
	if(min != min_val){
		min = min_val;
		update();
	}
}

void axis::set_max(float max_val){
	if(max != max_val){
		max = max_val;
		update();
	}
}

void axis::set_origin(glm::vec2 origin_val){
	if(origin != origin_val){
		axis::origin = origin_val;
		update();
	}
}
void axis::draw(){
	lines.draw();
	descr_text->draw();
	heads.draw();
	for(auto& label : seg_labels){
		label->draw();
	}
}

/**************************************************************/

x_axis::x_axis(
	std::string description,
	uint16_t length,
	float min,
	float max,
	float seg_width,
	uint8_t nr_subdiv,
	std::function<std::string(float)> label_func): axis(
		description,
		length,
		min,
		max,
		seg_width,
		nr_subdiv,
		'x',
		label_func){
	update();
}

/**************************************************************/

y_axis::y_axis(
	std::string description,
	uint16_t length,
	float min,
	float max,
	float seg_width,
	uint8_t nr_subdiv,
	std::function<std::string(float)> label_func): axis(
		description,
		length,
		min,
		max,
		seg_width,
		nr_subdiv,
		'y',
		label_func){
	update();
}
