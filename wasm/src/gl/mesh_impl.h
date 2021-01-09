#ifndef _MESH_IMPL_H_
#define _MESH_IMPL_H_

#include "mesh.h"

template<typename T>
void mesh<T>::ctx_begin(){
	((context&)*vertex_buffer).ctx_begin();
	if(index_buffer){
		((context&)*index_buffer).ctx_begin();
	}
	((context&)*prog).ctx_begin();
	for(auto& vertex_attribute : vertex_attributes){
        glVertexAttribPointer(
        	vertex_attribute.index,
        	vertex_attribute.element_count,
        	vertex_attribute.element_type,
        	GL_FALSE,
        	total_attributes_size,
        	(const void*)(vertex_attribute.offset));
        glEnableVertexAttribArray(vertex_attribute.index);
	}
	begin_func(prog);
}

template<typename T>
void mesh<T>::ctx_end(){
	end_func(prog);
	for(auto& vertex_attribute : vertex_attributes){
        glDisableVertexAttribArray(vertex_attribute.index);
	}
	((context&)*prog).ctx_end();
	if(index_buffer){
		((context&)*index_buffer).ctx_end();
	}
	((context&)*vertex_buffer).ctx_end();
}

template<typename T>
mesh<T>::mesh(
	std::shared_ptr<T> vertices,
	std::vector<GLuint>& indices,
	std::shared_ptr<program> prog,
	std::vector<vertex_attribute> in_vertex_attributes,
	GLenum drawing_mode,
	std::function<void(std::shared_ptr<program> prog)> begin_func,
	std::function<void(std::shared_ptr<program> prog)> end_func):
		prog(prog),
		drawing_mode(drawing_mode),
		begin_func(begin_func),
		end_func(end_func){
	
	total_attributes_size = 0;

	for(auto& vertex_attribute : in_vertex_attributes){
		vertex_attributes.push_back({
			prog->get_attrib(vertex_attribute.name),
			vertex_attribute.element_count,
			vertex_attribute.element_type,
			vertex_attribute.attribute_size,
			0
		});
		total_attributes_size += vertex_attribute.attribute_size;
	}
	
	std::sort(vertex_attributes.begin(), vertex_attributes.end(),[](i_vertex_attribute& a, i_vertex_attribute& b){
		return b.index > a.index;
	});
	
	size_t accumulated_size = 0;
	for(auto vertex_attribute = vertex_attributes.begin(); vertex_attribute != vertex_attributes.end(); ++vertex_attribute){
		vertex_attribute->offset = accumulated_size;
		accumulated_size += vertex_attribute->attribute_size;
	}

    vertex_buffer = vertices;
    if(indices.size()){
	    index_buffer = gl_um_buffer<GLuint>::create(indices, GL_ELEMENT_ARRAY_BUFFER);
	}
}

template<typename T>
void mesh<T>::draw(){
	ctx_begin();
	if(index_buffer){
    	glDrawElements(drawing_mode, index_buffer->size, GL_UNSIGNED_INT, nullptr);
	}else{
		glDrawArrays(drawing_mode, 0, vertex_buffer->size);
	}
	ctx_end();
}

#endif
