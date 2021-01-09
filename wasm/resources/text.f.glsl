in vec2 tex_coord;
uniform sampler2D text_texture;
out vec4 color;

void main(void) {
	color = texture(text_texture, tex_coord);
}
