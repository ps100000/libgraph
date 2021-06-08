flat in int fs_color_index;

out vec4 color;

uniform sampler2D color_map;

void main(void) {
	color = texelFetch(color_map, ivec2(fs_color_index, 0), 0);
}
