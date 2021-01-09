#include "texture.h"

#include <iostream>

texture::texture(
	GLint internalformat,
	GLenum format,
	GLsizei width,
	GLsizei height,
	GLenum type,
	const void * data,
	bool interpolate,
	bool repeat
	): gl_object() {
		GLint former_active_texture;
		glGetIntegerv(GL_ACTIVE_TEXTURE, &former_active_texture);

        glGenTextures(1, &gl_id);
        glActiveTexture(GL_TEXTURE0 + (gl_id % max_units));
        glBindTexture(GL_TEXTURE_2D, gl_id);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolate ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolate ? GL_LINEAR : GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);

        glActiveTexture(former_active_texture);
}

void texture::ctx_begin(){
	glGetIntegerv(GL_ACTIVE_TEXTURE, &previous_texture);
	glActiveTexture(GL_TEXTURE0 + (gl_id % max_units));
    glBindTexture(GL_TEXTURE_2D, gl_id);
}

void texture::ctx_end(){
	glActiveTexture(previous_texture);
}

texture::~texture(){
	glDeleteTextures(1, &gl_id);
}

texture::operator GLuint(){
	return gl_id % max_units;
}

void texture::init(){
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_units);
}

std::shared_ptr<texture> texture::create(
	GLint internalformat,
	GLenum format,
	GLsizei width,
	GLsizei height,
	GLenum type,
	const void * data,
	bool interpolate,
	bool repeat){
	return std::shared_ptr<texture>(new texture(
		internalformat,
		format,
		width,
		height,
		type,
		data,
		interpolate,
		repeat));
}
