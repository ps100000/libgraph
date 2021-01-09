#ifndef _VERTEX_ARRAY_H_
#define _VERTEX_ARRAY_H_
#include <vector>
#include <memory>
#include <string>

#include <GL/glew.h>

#include "gl_object.h"
#include "context.h"

class vertex_array: public gl_object, public context{
	public:
		struct vertex_attribute{
			std::string name;
			GLint element_count;
			GLenum element_type;
			size_t attribute_size;
		};
	private:
		vertex_array(std::vector<vertex_attribute> attributes);
		
		void ctx_begin() override;
		void ctx_end() override;
	public:
		~vertex_array();

		static std::shared_ptr<vertex_array> create(std::vector<vertex_attribute> attributes);
};

#endif
