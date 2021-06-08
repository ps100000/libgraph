#ifndef _MESH_H_
#define _MESH_H_
#include <memory>
#include <functional>

#include <GL/glew.h>

#include "context.h"
#include "program.h"
#include "gl_buffer_impl.h"

template<typename T>
class mesh: public context{
	public:
		struct vertex_attribute{
			std::string name;
			GLint element_count;
			GLenum element_type;
			size_t attribute_size;
		};
	private:
		struct i_vertex_attribute{
			GLint index;
			GLint element_count;
			GLenum element_type;
			size_t attribute_size;
			size_t offset;
		};
        std::shared_ptr<program> prog;
		std::vector<i_vertex_attribute> vertex_attributes; // todo: vertex array class
		size_t total_attributes_size;

		void ctx_begin() override;
		void ctx_end() override;
	public:
        std::shared_ptr<T> vertex_buffer;
        std::shared_ptr<gl_um_buffer<GLuint>> index_buffer;
        GLenum drawing_mode;

		std::function<void(std::shared_ptr<program> prog)> begin_func;
		std::function<void(std::shared_ptr<program> prog)> end_func;

		mesh() = default;
		mesh(
			std::shared_ptr<T> vertices,
			std::vector<GLuint>& indices,
			std::shared_ptr<program> prog,
			std::vector<vertex_attribute> vertex_attributes,
			GLenum drawing_mode = GL_TRIANGLES,
			std::function<void(std::shared_ptr<program> prog)> begin_func =
				(std::function<void(std::shared_ptr<program>)>)[](std::shared_ptr<program>){},
			std::function<void(std::shared_ptr<program> prog)> end_func =
				(std::function<void(std::shared_ptr<program>)>)[](std::shared_ptr<program>){});

		void draw();
		void draw(size_t offset, size_t count);
};

#endif
