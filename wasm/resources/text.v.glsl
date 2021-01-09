in vec2 a_coord;
in vec2 a_tex_coord;
out vec2 tex_coord;
uniform float width;
uniform float height;
uniform vec2 pos;

void main(void) {
	tex_coord = vec2(a_tex_coord.x, 1.0-a_tex_coord.y);
	vec2 true_coord = a_coord + pos;
	gl_Position = vec4(true_coord.x / width - 0.5, true_coord.y / height - 0.5, 0, 0.5);
}
