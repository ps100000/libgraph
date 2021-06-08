in vec2 a_coord;
in float color_index;

flat out int fs_color_index;

uniform float width;
uniform float height;
uniform vec2 origin;

void main(void) {
	fs_color_index = int(color_index);

	vec2 true_coord = a_coord + origin;
	gl_Position = vec4(true_coord.x / width - 0.5, true_coord.y / height - 0.5, 0, 0.5);
}
